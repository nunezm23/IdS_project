#include "acquisition.h"
#include "ads1263.h"
#include "app_log.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static unsigned int emitted_samples;

ads1263_status_t ads1263_init(ads1263_t *dev, const ads1263_config_t *config)
{
    (void)dev;
    (void)config;
    return ADS1263_OK;
}

void ads1263_close(ads1263_t *dev)
{
    (void)dev;
}

ads1263_status_t ads1263_check_presence(ads1263_t *dev,
                                        ads1263_register_snapshot_t *snapshot)
{
    (void)dev;
    memset(snapshot, 0, sizeof(*snapshot));
    return ADS1263_OK;
}

ads1263_status_t ads1263_configure_channel(ads1263_t *dev, uint8_t channel)
{
    (void)dev;
    (void)channel;
    return ADS1263_OK;
}

ads1263_status_t ads1263_read_sample(ads1263_t *dev, ads1263_sample_t *sample)
{
    (void)dev;
    memset(sample, 0, sizeof(*sample));
    sample->status = "OK";
    return ADS1263_OK;
}

ads1263_status_t ads1263_read_drdy(ads1263_t *dev, int *value)
{
    (void)dev;
    *value = 0;
    return ADS1263_OK;
}

const char *ads1263_status_to_string(ads1263_status_t status)
{
    (void)status;
    return "UNUSED";
}

void app_log_info(const char *fmt, ...)
{
    (void)fmt;
}

void app_log_warn(const char *fmt, ...)
{
    (void)fmt;
}

void app_log_error(const char *fmt, ...)
{
    (void)fmt;
}

void app_log_sample(uint32_t sample_index, const ads1263_sample_t *sample)
{
    (void)sample_index;
    (void)sample;
    emitted_samples++;
}

void app_log_csv_sample(FILE *stream,
                        uint32_t sample_index,
                        const ads1263_sample_t *sample)
{
    (void)stream;
    (void)sample_index;
    (void)sample;
}

int main(void)
{
    ads1263_config_t adc;
    acquisition_config_t acq;
    int result;

    memset(&adc, 0, sizeof(adc));
    adc.spi_device = "/dev/fake-spi";
    adc.gpio_chip = "fakechip";
    adc.spi_speed_hz = 1000000U;
    adc.gain = 1U;

    memset(&acq, 0, sizeof(acq));
    acq.samples = 2U;
    acq.rate_hz = 1U;

    puts("DSP-017 probe: starting N=2 rate_hz=1");
    fflush(stdout);
    result = acquisition_run_samples(&adc, &acq);
    printf("DSP-017 probe: result=%d samples=%u\n", result, emitted_samples);
    return result == 0 ? 0 : 1;
}
