#include "acquisition.h"
#include "ads1263.h"
#include "app_log.h"

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
    APP_MODE_NONE = 0,
    APP_MODE_HELP,
    APP_MODE_CHECK,
    APP_MODE_SAMPLES,
    APP_MODE_LIVE
} app_mode_t;

static void print_usage(void)
{
    puts("Usage:");
    puts("  ads1263_app --check");
    puts("  ads1263_app --samples <N> --channel <CH> --rate-hz <HZ> [--csv <path>]");
    puts("  ads1263_app --live --channel <CH> --rate-hz <HZ>");
    puts("  ads1263_app --help");
    puts("");
    puts("Defaults:");
    puts("  spi device  : /dev/spidev0.0");
    puts("  gpio chip   : /dev/gpiochip6");
    puts("  drdy line   : 8");
    puts("  samples     : 10");
    puts("  rate-hz     : 2");
    puts("  channel     : 0 (AIN0-AINCOM)");
    puts("");
    puts("Advanced:");
    puts("  --spi-dev <path>       Override SPI character device");
    puts("  --gpio-chip <path>     Override GPIO chip path");
    puts("  --drdy-line <offset>   Override DRDY GPIO line offset");
    puts("  --no-drdy              Skip DRDY GPIO open/read for SPI-only checks");
    puts("  --mock                 Simulate ADS1263 registers and samples");
}

static int parse_u32_range(const char *option,
                           const char *value,
                           uint32_t min,
                           uint32_t max,
                           uint32_t *out)
{
    char *end = NULL;
    unsigned long parsed;

    if (value == NULL || value[0] == '\0') {
        app_log_error("%s requires a value", option);
        return -1;
    }

    errno = 0;
    parsed = strtoul(value, &end, 10);
    if (errno != 0 || end == value || *end != '\0' || parsed < min || parsed > max) {
        app_log_error("invalid %s value. Valid range: %u..%u", option, min, max);
        return -1;
    }

    *out = (uint32_t)parsed;
    return 0;
}

static int require_next_arg(int argc, char **argv, int *index, const char *option, const char **out)
{
    if (*index + 1 >= argc) {
        app_log_error("%s requires a value", option);
        return -1;
    }

    *index += 1;
    *out = argv[*index];
    return 0;
}

int main(int argc, char **argv)
{
    app_mode_t mode = APP_MODE_NONE;
    int mode_count = 0;
    int errors = 0;

    ads1263_config_t adc_config = ads1263_default_config();
    acquisition_config_t acq_config = acquisition_default_config();

    if (argc == 1) {
        print_usage();
        return EXIT_FAILURE;
    }

    for (int i = 1; i < argc; ++i) {
        const char *arg = argv[i];
        const char *value = NULL;
        uint32_t parsed = 0;

        if (strcmp(arg, "--help") == 0) {
            mode = APP_MODE_HELP;
            mode_count++;
        } else if (strcmp(arg, "--check") == 0) {
            mode = APP_MODE_CHECK;
            mode_count++;
        } else if (strcmp(arg, "--samples") == 0) {
            mode = APP_MODE_SAMPLES;
            mode_count++;
            if (require_next_arg(argc, argv, &i, arg, &value) != 0 ||
                parse_u32_range("samples", value, 1U, 1000U, &parsed) != 0) {
                errors++;
            } else {
                acq_config.samples = parsed;
            }
        } else if (strcmp(arg, "--live") == 0) {
            mode = APP_MODE_LIVE;
            mode_count++;
            acq_config.live = 1;
        } else if (strcmp(arg, "--channel") == 0) {
            if (require_next_arg(argc, argv, &i, arg, &value) != 0 ||
                parse_u32_range("channel", value, 0U, 0U, &parsed) != 0) {
                errors++;
            } else {
                acq_config.channel = (uint8_t)parsed;
            }
        } else if (strcmp(arg, "--rate-hz") == 0) {
            if (require_next_arg(argc, argv, &i, arg, &value) != 0 ||
                parse_u32_range("rate-hz", value, 1U, 10U, &parsed) != 0) {
                errors++;
            } else {
                acq_config.rate_hz = parsed;
            }
        } else if (strcmp(arg, "--csv") == 0) {
            if (require_next_arg(argc, argv, &i, arg, &value) != 0) {
                errors++;
            } else {
                acq_config.csv_path = value;
            }
        } else if (strcmp(arg, "--spi-dev") == 0) {
            if (require_next_arg(argc, argv, &i, arg, &value) != 0) {
                errors++;
            } else {
                adc_config.spi_device = value;
            }
        } else if (strcmp(arg, "--gpio-chip") == 0) {
            if (require_next_arg(argc, argv, &i, arg, &value) != 0) {
                errors++;
            } else {
                adc_config.gpio_chip = value;
            }
        } else if (strcmp(arg, "--drdy-line") == 0) {
            if (require_next_arg(argc, argv, &i, arg, &value) != 0 ||
                parse_u32_range("drdy-line", value, 0U, 1023U, &parsed) != 0) {
                errors++;
            } else {
                adc_config.drdy_line = parsed;
            }
        } else if (strcmp(arg, "--no-drdy") == 0) {
            adc_config.use_drdy = 0;
        } else if (strcmp(arg, "--mock") == 0) {
            adc_config.mock = 1;
            adc_config.use_drdy = 0;
        } else {
            app_log_error("unknown option: %s", arg);
            errors++;
        }
    }

    if (mode == APP_MODE_HELP) {
        print_usage();
        return errors == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    if (mode_count != 1) {
        app_log_error("select exactly one command mode: --check, --samples, --live, or --help");
        errors++;
    }

    if (mode == APP_MODE_LIVE && acq_config.csv_path != NULL) {
        app_log_error("--csv is supported with --samples mode only");
        errors++;
    }

    if (errors != 0) {
        return EXIT_FAILURE;
    }

    switch (mode) {
    case APP_MODE_CHECK:
        return acquisition_run_check(&adc_config) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    case APP_MODE_SAMPLES:
        acq_config.live = 0;
        return acquisition_run_samples(&adc_config, &acq_config) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    case APP_MODE_LIVE:
        acq_config.live = 1;
        return acquisition_run_live(&adc_config, &acq_config) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
    case APP_MODE_HELP:
    case APP_MODE_NONE:
    default:
        print_usage();
        return EXIT_FAILURE;
    }
}
