#ifndef ADS1263_DEFS_H
#define ADS1263_DEFS_H

#include <stdint.h>

#define ADS1263_CMD_NOP       0x00U
#define ADS1263_CMD_RESET     0x06U
#define ADS1263_CMD_START1    0x08U
#define ADS1263_CMD_STOP1     0x0AU
#define ADS1263_CMD_RDATA1    0x12U
#define ADS1263_CMD_RREG      0x20U
#define ADS1263_CMD_WREG      0x40U

#define ADS1263_REG_ID        0x00U
#define ADS1263_REG_POWER     0x01U
#define ADS1263_REG_INTERFACE 0x02U
#define ADS1263_REG_MODE0     0x03U
#define ADS1263_REG_MODE1     0x04U
#define ADS1263_REG_MODE2     0x05U
#define ADS1263_REG_INPMUX    0x06U

#define ADS1263_DEFAULT_POWER     0x11U
#define ADS1263_DEFAULT_INTERFACE 0x05U
#define ADS1263_DEFAULT_MODE0     0x00U
#define ADS1263_DEFAULT_MODE1     0x80U

#define ADS1263_DEFAULT_SPI_DEVICE   "/dev/spidev0.0"
#define ADS1263_DEFAULT_GPIO_CHIP    "/dev/gpiochip6"
#define ADS1263_DEFAULT_DRDY_LINE    8U
#define ADS1263_DEFAULT_SPI_SPEED_HZ 1000000U
#define ADS1263_DEFAULT_VREF         2.5
#define ADS1263_DEFAULT_GAIN         1U

#define ADS1263_INPUT_AIN0   0x00U
#define ADS1263_INPUT_AINCOM 0x0AU
#define ADS1263_INPMUX_AIN0_AINCOM ((uint8_t)((ADS1263_INPUT_AIN0 << 4U) | ADS1263_INPUT_AINCOM))

#define ADS1263_ADC1_POSITIVE_FULL_SCALE 2147483647.0

typedef enum {
    ADS1263_OK = 0,
    ADS1263_ERROR_SPI_OPEN,
    ADS1263_ERROR_SPI_CONFIG,
    ADS1263_ERROR_SPI_TRANSFER,
    ADS1263_ERROR_GPIO_OPEN,
    ADS1263_ERROR_GPIO_READ,
    ADS1263_ERROR_NOT_DETECTED,
    ADS1263_ERROR_NOT_DETECTED_ZERO,
    ADS1263_ERROR_NOT_DETECTED_FLOATING,
    ADS1263_ERROR_REGISTER_MISMATCH,
    ADS1263_ERROR_INVALID_ARG,
    ADS1263_ERROR_TIMEOUT,
    ADS1263_ERROR_IO
} ads1263_status_t;

typedef struct {
    int32_t raw;
    double voltage;
    int drdy;
    uint8_t status_byte;
    uint8_t checksum_byte;
    const char *status;
} ads1263_sample_t;

typedef struct {
    uint8_t id;
    uint8_t power;
    uint8_t interface;
    uint8_t mode0;
    uint8_t mode1;
} ads1263_register_snapshot_t;

typedef struct {
    const char *spi_device;
    const char *gpio_chip;
    unsigned int drdy_line;
    uint32_t spi_speed_hz;
    double vref;
    uint8_t gain;
    int use_drdy;
    int mock;
} ads1263_config_t;

#endif /* ADS1263_DEFS_H */
