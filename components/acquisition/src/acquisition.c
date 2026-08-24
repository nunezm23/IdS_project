#include "acquisition.h"

#include "ads1263.h"
#include "app_log.h"

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

static volatile sig_atomic_t g_stop_requested;

acquisition_config_t acquisition_default_config(void)
{
    acquisition_config_t config;

    config.channel = 0;
    config.samples = 10;
    config.rate_hz = 2;
    config.live = 0;
    config.csv_path = NULL;

    return config;
}

static void print_snapshot(const ads1263_register_snapshot_t *snapshot)
{
    if (snapshot == NULL) {
        return;
    }

    printf("ID        = 0x%02X\n", snapshot->id);
    printf("POWER     = 0x%02X\n", snapshot->power);
    printf("INTERFACE = 0x%02X\n", snapshot->interface);
    printf("MODE0     = 0x%02X\n", snapshot->mode0);
    printf("MODE1     = 0x%02X\n", snapshot->mode1);
}

static void log_presence_error(ads1263_status_t status,
                               const ads1263_register_snapshot_t *snapshot)
{
    switch (status) {
    case ADS1263_ERROR_NOT_DETECTED_ZERO:
        app_log_error("ADS1263 not detected: all registers read as 0x00");
        app_log_error("Check SPI wiring, ADS1263 power, GND, CS, MISO, MOSI, SCLK and SPI mode.");
        break;
    case ADS1263_ERROR_NOT_DETECTED_FLOATING:
        app_log_error("ADS1263 not detected: all registers read as 0xFF");
        app_log_error("Check floating MISO, CS wiring, ADS1263 power, GND and SPI mode.");
        break;
    case ADS1263_ERROR_REGISTER_MISMATCH:
        app_log_error("ADS1263 register baseline mismatch");
        if (snapshot != NULL) {
            app_log_error("Expected POWER=0x%02X INTERFACE=0x%02X MODE0=0x%02X MODE1=0x%02X",
                          ADS1263_DEFAULT_POWER,
                          ADS1263_DEFAULT_INTERFACE,
                          ADS1263_DEFAULT_MODE0,
                          ADS1263_DEFAULT_MODE1);
        }
        break;
    default:
        app_log_error("ADS1263 check failed: %s", ads1263_status_to_string(status));
        break;
    }
}

static int log_init_error(ads1263_status_t status, const ads1263_config_t *config)
{
    if (status == ADS1263_ERROR_SPI_OPEN) {
        app_log_error("failed to open SPI device: %s", config->spi_device);
        app_log_error("If /dev/spidev0.0 is missing, bind spi0.0 to spidev first.");
    } else if (status == ADS1263_ERROR_GPIO_OPEN) {
        app_log_error("failed to open DRDY GPIO: chip=%s line=%u",
                      config->gpio_chip,
                      config->drdy_line);
        app_log_error("Check gpioinfo -c gpiochip6 and verify PG8 is available.");
    } else {
        app_log_error("failed to initialize ADS1263: %s", ads1263_status_to_string(status));
    }

    return -1;
}

int acquisition_run_check(const ads1263_config_t *adc_config)
{
    ads1263_t adc;
    ads1263_register_snapshot_t snapshot;
    ads1263_status_t status;
    int drdy = -1;

    if (adc_config == NULL) {
        app_log_error("invalid ADC configuration");
        return -1;
    }

    status = ads1263_init(&adc, adc_config);
    if (status != ADS1263_OK) {
        return log_init_error(status, adc_config);
    }

    app_log_info("ADS1263 register check");
    status = ads1263_check_presence(&adc, &snapshot);
    print_snapshot(&snapshot);

    if (status != ADS1263_OK) {
        log_presence_error(status, &snapshot);
        ads1263_close(&adc);
        return -1;
    }

    app_log_info("ADS1263 detected");

    status = ads1263_read_drdy(&adc, &drdy);
    if (status == ADS1263_OK) {
        app_log_info("DRDY: chip=%s line=%u value=%d",
                     adc_config->gpio_chip,
                     adc_config->drdy_line,
                     drdy);
    } else {
        app_log_warn("DRDY read failed: %s", ads1263_status_to_string(status));
    }

    ads1263_close(&adc);
    return 0;
}

static int validate_acquisition_config(const acquisition_config_t *config)
{
    int errors = 0;

    if (config == NULL) {
        app_log_error("invalid acquisition configuration");
        return -1;
    }

    if (config->samples < 1U || config->samples > 1000U) {
        app_log_error("invalid samples value. Valid range: 1..1000");
        errors++;
    }

    if (config->rate_hz < 1U || config->rate_hz > 10U) {
        app_log_error("invalid rate-hz value. Valid range: 1..10");
        errors++;
    }

    if (config->channel != 0U) {
        app_log_error("invalid channel value. Valid range: 0..0");
        errors++;
    }

    return errors == 0 ? 0 : -1;
}

static void sleep_for_rate(uint32_t rate_hz)
{
    struct timespec req;

    if (rate_hz == 0U) {
        return;
    }

    req.tv_sec = 0;
    req.tv_nsec = 1000000000L / (long)rate_hz;
    while (nanosleep(&req, &req) != 0) {
    }
}

static FILE *open_csv_if_requested(const acquisition_config_t *config)
{
    FILE *csv = NULL;

    if (config->csv_path == NULL) {
        return NULL;
    }

    csv = fopen(config->csv_path, "w");
    if (csv == NULL) {
        app_log_warn("could not open CSV path: %s", config->csv_path);
        app_log_warn("continuing with console output only");
        return NULL;
    }

    fputs("sample,raw,voltage,status\n", csv);
    return csv;
}

static int prepare_adc_for_acquisition(ads1263_t *adc,
                                       const ads1263_config_t *adc_config,
                                       const acquisition_config_t *acq_config)
{
    ads1263_register_snapshot_t snapshot;
    ads1263_status_t status;

    status = ads1263_init(adc, adc_config);
    if (status != ADS1263_OK) {
        return log_init_error(status, adc_config);
    }

    status = ads1263_check_presence(adc, &snapshot);
    if (status != ADS1263_OK) {
        print_snapshot(&snapshot);
        log_presence_error(status, &snapshot);
        ads1263_close(adc);
        return -1;
    }

    status = ads1263_configure_channel(adc, acq_config->channel);
    if (status != ADS1263_OK) {
        app_log_error("failed to configure ADS1263 channel %u: %s",
                      acq_config->channel,
                      ads1263_status_to_string(status));
        ads1263_close(adc);
        return -1;
    }

    return 0;
}

static int acquire_loop(ads1263_t *adc,
                        const acquisition_config_t *config,
                        uint32_t max_samples,
                        int live)
{
    FILE *csv = NULL;
    uint32_t sample_index = 1;
    int rc = 0;

    csv = open_csv_if_requested(config);

    while ((live != 0 && g_stop_requested == 0) ||
           (live == 0 && sample_index <= max_samples)) {
        ads1263_sample_t sample;
        ads1263_status_t status;

        status = ads1263_read_sample(adc, &sample);
        if (status != ADS1263_OK) {
            app_log_error("sample read failed: %s", ads1263_status_to_string(status));
            rc = -1;
            break;
        }

        app_log_sample(sample_index, &sample);
        app_log_csv_sample(csv, sample_index, &sample);

        sample_index++;
        if (live == 0 && sample_index > max_samples) {
            break;
        }

        sleep_for_rate(config->rate_hz);
    }

    if (csv != NULL) {
        fclose(csv);
    }

    return rc;
}

int acquisition_run_samples(const ads1263_config_t *adc_config,
                            const acquisition_config_t *acq_config)
{
    ads1263_t adc;
    int rc;

    if (adc_config == NULL || validate_acquisition_config(acq_config) != 0) {
        return -1;
    }

    app_log_info("Acquisition configuration: channel=%u samples=%u rate_hz=%u",
                 acq_config->channel,
                 acq_config->samples,
                 acq_config->rate_hz);

    if (prepare_adc_for_acquisition(&adc, adc_config, acq_config) != 0) {
        return -1;
    }

    rc = acquire_loop(&adc, acq_config, acq_config->samples, 0);
    ads1263_close(&adc);

    return rc;
}

static void handle_sigint(int signum)
{
    (void)signum;
    g_stop_requested = 1;
}

int acquisition_run_live(const ads1263_config_t *adc_config,
                         const acquisition_config_t *acq_config)
{
    ads1263_t adc;
    int rc;

    if (adc_config == NULL || validate_acquisition_config(acq_config) != 0) {
        return -1;
    }

    g_stop_requested = 0;
    (void)signal(SIGINT, handle_sigint);

    app_log_info("Live acquisition configuration: channel=%u rate_hz=%u",
                 acq_config->channel,
                 acq_config->rate_hz);

    if (prepare_adc_for_acquisition(&adc, adc_config, acq_config) != 0) {
        return -1;
    }

    rc = acquire_loop(&adc, acq_config, 0, 1);
    ads1263_close(&adc);
    app_log_info("live acquisition stopped");

    return rc;
}
