#ifndef ADS1263_GPIO_LINUX_H
#define ADS1263_GPIO_LINUX_H

#include "ads1263_defs.h"

struct gpiod_chip;
struct gpiod_line_request;

typedef struct {
    struct gpiod_chip *chip;
    struct gpiod_line_request *request;
    unsigned int line;
    int enabled;
} ads1263_gpio_linux_t;

/**
 * @brief Open a Linux GPIO character device and request one DRDY input line.
 */
ads1263_status_t ads1263_gpio_linux_open(ads1263_gpio_linux_t *gpio,
                                         const char *chip_path,
                                         unsigned int line);

/**
 * @brief Read the logical DRDY line value.
 */
ads1263_status_t ads1263_gpio_linux_read_drdy(ads1263_gpio_linux_t *gpio, int *value);

/**
 * @brief Release the DRDY line request and close the GPIO chip.
 */
void ads1263_gpio_linux_close(ads1263_gpio_linux_t *gpio);

#endif /* ADS1263_GPIO_LINUX_H */
