#ifndef ADS1263_H
#define ADS1263_H

#include "ads1263_defs.h"
#include "ads1263_gpio_linux.h"
#include "ads1263_spi_linux.h"

#include <stddef.h>
#include <stdint.h>

typedef struct {
    ads1263_config_t config;
    ads1263_spi_linux_t spi;
    ads1263_gpio_linux_t gpio;
    int initialized;
    uint8_t mock_registers[32];
    uint32_t mock_sample_counter;
} ads1263_t;

/**
 * @brief Return the default STM32MP157D-DK1 demo configuration.
 */
ads1263_config_t ads1263_default_config(void);

/**
 * @brief Initialize Linux SPI and optional DRDY GPIO resources.
 */
ads1263_status_t ads1263_init(ads1263_t *dev, const ads1263_config_t *config);

/**
 * @brief Release Linux resources associated with the ADS1263 instance.
 */
void ads1263_close(ads1263_t *dev);

/**
 * @brief Read one or more ADS1263 registers using the RREG command.
 */
ads1263_status_t ads1263_read_registers(ads1263_t *dev,
                                        uint8_t start,
                                        uint8_t count,
                                        uint8_t *out);

/**
 * @brief Write one or more ADS1263 registers using the WREG command.
 */
ads1263_status_t ads1263_write_registers(ads1263_t *dev,
                                         uint8_t start,
                                         uint8_t count,
                                         const uint8_t *values);

/**
 * @brief Read and validate ADS1263 identity/default registers.
 */
ads1263_status_t ads1263_check_presence(ads1263_t *dev,
                                        ads1263_register_snapshot_t *snapshot);

/**
 * @brief Configure the acquisition channel. Version 1 supports channel 0 only.
 */
ads1263_status_t ads1263_configure_channel(ads1263_t *dev, uint8_t channel);

/**
 * @brief Read one ADC1 conversion sample using RDATA1.
 */
ads1263_status_t ads1263_read_sample(ads1263_t *dev, ads1263_sample_t *sample);

/**
 * @brief Read the DRDY GPIO line as a logical value.
 */
ads1263_status_t ads1263_read_drdy(ads1263_t *dev, int *value);

/**
 * @brief Convert signed ADC1 raw data to an approximate voltage.
 */
double ads1263_raw_to_voltage(int32_t raw, double vref, uint8_t gain);

/**
 * @brief Return a stable string for a status code.
 */
const char *ads1263_status_to_string(ads1263_status_t status);

#endif /* ADS1263_H */
