#include "ads1263_gpio_linux.h"

#include <string.h>

#if ADS1263_NO_GPIOD

ads1263_status_t ads1263_gpio_linux_open(ads1263_gpio_linux_t *gpio,
                                         const char *chip_path,
                                         unsigned int line)
{
    (void)chip_path;

    if (gpio == NULL) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    memset(gpio, 0, sizeof(*gpio));
    gpio->line = line;
    gpio->enabled = 0;
    return ADS1263_ERROR_GPIO_OPEN;
}

ads1263_status_t ads1263_gpio_linux_read_drdy(ads1263_gpio_linux_t *gpio, int *value)
{
    (void)gpio;
    (void)value;
    return ADS1263_ERROR_GPIO_READ;
}

void ads1263_gpio_linux_close(ads1263_gpio_linux_t *gpio)
{
    (void)gpio;
}

#else

#include <gpiod.h>

ads1263_status_t ads1263_gpio_linux_open(ads1263_gpio_linux_t *gpio,
                                         const char *chip_path,
                                         unsigned int line)
{
    struct gpiod_request_config *request_config = NULL;
    struct gpiod_line_config *line_config = NULL;
    struct gpiod_line_settings *settings = NULL;
    const unsigned int offsets[] = { line };
    ads1263_status_t status = ADS1263_ERROR_GPIO_OPEN;

    if (gpio == NULL || chip_path == NULL) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    memset(gpio, 0, sizeof(*gpio));
    gpio->line = line;

    gpio->chip = gpiod_chip_open(chip_path);
    if (gpio->chip == NULL) {
        goto cleanup;
    }

    settings = gpiod_line_settings_new();
    line_config = gpiod_line_config_new();
    request_config = gpiod_request_config_new();
    if (settings == NULL || line_config == NULL || request_config == NULL) {
        goto cleanup;
    }

    if (gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_INPUT) != 0) {
        goto cleanup;
    }

    if (gpiod_line_config_add_line_settings(line_config, offsets, 1U, settings) != 0) {
        goto cleanup;
    }

    gpiod_request_config_set_consumer(request_config, "ads1263_app");

    gpio->request = gpiod_chip_request_lines(gpio->chip, request_config, line_config);
    if (gpio->request == NULL) {
        goto cleanup;
    }

    gpio->enabled = 1;
    status = ADS1263_OK;

cleanup:
    if (request_config != NULL) {
        gpiod_request_config_free(request_config);
    }
    if (line_config != NULL) {
        gpiod_line_config_free(line_config);
    }
    if (settings != NULL) {
        gpiod_line_settings_free(settings);
    }
    if (status != ADS1263_OK) {
        ads1263_gpio_linux_close(gpio);
    }
    return status;
}

ads1263_status_t ads1263_gpio_linux_read_drdy(ads1263_gpio_linux_t *gpio, int *value)
{
    enum gpiod_line_value line_value;

    if (gpio == NULL || value == NULL || gpio->request == NULL) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    line_value = gpiod_line_request_get_value(gpio->request, gpio->line);
    if (line_value == GPIOD_LINE_VALUE_ERROR) {
        return ADS1263_ERROR_GPIO_READ;
    }

    *value = (line_value == GPIOD_LINE_VALUE_ACTIVE) ? 1 : 0;
    return ADS1263_OK;
}

void ads1263_gpio_linux_close(ads1263_gpio_linux_t *gpio)
{
    if (gpio == NULL) {
        return;
    }

    if (gpio->request != NULL) {
        gpiod_line_request_release(gpio->request);
    }
    if (gpio->chip != NULL) {
        gpiod_chip_close(gpio->chip);
    }

    gpio->request = NULL;
    gpio->chip = NULL;
    gpio->enabled = 0;
}

#endif
