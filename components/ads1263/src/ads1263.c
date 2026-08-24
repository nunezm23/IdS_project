#include "ads1263.h"

#include <stdint.h>
#include <string.h>
#include <time.h>

ads1263_config_t ads1263_default_config(void)
{
    ads1263_config_t config;

    config.spi_device = ADS1263_DEFAULT_SPI_DEVICE;
    config.gpio_chip = ADS1263_DEFAULT_GPIO_CHIP;
    config.drdy_line = ADS1263_DEFAULT_DRDY_LINE;
    config.spi_speed_hz = ADS1263_DEFAULT_SPI_SPEED_HZ;
    config.vref = ADS1263_DEFAULT_VREF;
    config.gain = ADS1263_DEFAULT_GAIN;
    config.use_drdy = 1;
    config.mock = 0;

    return config;
}

static void ads1263_mock_registers_reset(ads1263_t *dev)
{
    if (dev == NULL) {
        return;
    }

    memset(dev->mock_registers, 0, sizeof(dev->mock_registers));
    dev->mock_registers[ADS1263_REG_ID] = 0x23U;
    dev->mock_registers[ADS1263_REG_POWER] = ADS1263_DEFAULT_POWER;
    dev->mock_registers[ADS1263_REG_INTERFACE] = ADS1263_DEFAULT_INTERFACE;
    dev->mock_registers[ADS1263_REG_MODE0] = ADS1263_DEFAULT_MODE0;
    dev->mock_registers[ADS1263_REG_MODE1] = ADS1263_DEFAULT_MODE1;
    dev->mock_registers[ADS1263_REG_MODE2] = 0x04U;
    dev->mock_registers[ADS1263_REG_INPMUX] = 0x01U;
    dev->mock_sample_counter = 0U;
}

ads1263_status_t ads1263_init(ads1263_t *dev, const ads1263_config_t *config)
{
    ads1263_status_t status;

    if (dev == NULL || config == NULL || config->spi_device == NULL ||
        config->spi_speed_hz == 0U || config->gain == 0U) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    memset(dev, 0, sizeof(*dev));
    dev->config = *config;
    dev->spi.fd = -1;

    if (config->mock != 0) {
        ads1263_mock_registers_reset(dev);
        dev->initialized = 1;
        return ADS1263_OK;
    }

    status = ads1263_spi_linux_open(&dev->spi, config->spi_device, config->spi_speed_hz);
    if (status != ADS1263_OK) {
        return status;
    }

    if (config->use_drdy != 0) {
        if (config->gpio_chip == NULL) {
            ads1263_spi_linux_close(&dev->spi);
            return ADS1263_ERROR_INVALID_ARG;
        }

        status = ads1263_gpio_linux_open(&dev->gpio, config->gpio_chip, config->drdy_line);
        if (status != ADS1263_OK) {
            ads1263_spi_linux_close(&dev->spi);
            return status;
        }
    }

    dev->initialized = 1;
    return ADS1263_OK;
}

void ads1263_close(ads1263_t *dev)
{
    if (dev == NULL) {
        return;
    }

    ads1263_gpio_linux_close(&dev->gpio);
    ads1263_spi_linux_close(&dev->spi);
    dev->initialized = 0;
}

ads1263_status_t ads1263_read_registers(ads1263_t *dev,
                                        uint8_t start,
                                        uint8_t count,
                                        uint8_t *out)
{
    uint8_t tx[34];
    uint8_t rx[34];
    size_t len;
    ads1263_status_t status;

    if (dev == NULL || out == NULL || count == 0U || count > 32U || start > 0x1FU) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    if (dev->config.mock != 0) {
        memcpy(out, &dev->mock_registers[start], count);
        return ADS1263_OK;
    }

    len = (size_t)count + 2U;
    memset(tx, 0, sizeof(tx));
    memset(rx, 0, sizeof(rx));

    tx[0] = (uint8_t)(ADS1263_CMD_RREG | (start & 0x1FU));
    tx[1] = (uint8_t)(count - 1U);

    status = ads1263_spi_linux_transfer(&dev->spi, tx, rx, len);
    if (status != ADS1263_OK) {
        return status;
    }

    memcpy(out, &rx[2], count);
    return ADS1263_OK;
}

ads1263_status_t ads1263_write_registers(ads1263_t *dev,
                                         uint8_t start,
                                         uint8_t count,
                                         const uint8_t *values)
{
    uint8_t tx[34];
    uint8_t rx[34];
    size_t len;

    if (dev == NULL || values == NULL || count == 0U || count > 32U || start > 0x1FU) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    if (dev->config.mock != 0) {
        memcpy(&dev->mock_registers[start], values, count);
        return ADS1263_OK;
    }

    len = (size_t)count + 2U;
    memset(tx, 0, sizeof(tx));
    memset(rx, 0, sizeof(rx));

    tx[0] = (uint8_t)(ADS1263_CMD_WREG | (start & 0x1FU));
    tx[1] = (uint8_t)(count - 1U);
    memcpy(&tx[2], values, count);

    return ads1263_spi_linux_transfer(&dev->spi, tx, rx, len);
}

static int all_equal(const uint8_t *values, size_t len, uint8_t expected)
{
    for (size_t i = 0; i < len; ++i) {
        if (values[i] != expected) {
            return 0;
        }
    }

    return 1;
}

ads1263_status_t ads1263_check_presence(ads1263_t *dev,
                                        ads1263_register_snapshot_t *snapshot)
{
    uint8_t regs[5];
    ads1263_status_t status;

    if (dev == NULL || snapshot == NULL) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    memset(regs, 0, sizeof(regs));
    memset(snapshot, 0, sizeof(*snapshot));

    status = ads1263_read_registers(dev, ADS1263_REG_ID, (uint8_t)sizeof(regs), regs);
    if (status != ADS1263_OK) {
        return status;
    }

    snapshot->id = regs[0];
    snapshot->power = regs[1];
    snapshot->interface = regs[2];
    snapshot->mode0 = regs[3];
    snapshot->mode1 = regs[4];

    if (all_equal(regs, sizeof(regs), 0x00U) != 0) {
        return ADS1263_ERROR_NOT_DETECTED_ZERO;
    }

    if (all_equal(regs, sizeof(regs), 0xFFU) != 0) {
        return ADS1263_ERROR_NOT_DETECTED_FLOATING;
    }

    if (snapshot->power != ADS1263_DEFAULT_POWER ||
        snapshot->interface != ADS1263_DEFAULT_INTERFACE ||
        snapshot->mode0 != ADS1263_DEFAULT_MODE0 ||
        snapshot->mode1 != ADS1263_DEFAULT_MODE1) {
        return ADS1263_ERROR_REGISTER_MISMATCH;
    }

    return ADS1263_OK;
}

static ads1263_status_t ads1263_send_command(ads1263_t *dev, uint8_t command)
{
    uint8_t tx[1] = { command };
    uint8_t rx[1] = { 0 };

    if (dev == NULL) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    if (dev->config.mock != 0) {
        return ADS1263_OK;
    }

    return ads1263_spi_linux_transfer(&dev->spi, tx, rx, sizeof(tx));
}

ads1263_status_t ads1263_configure_channel(ads1263_t *dev, uint8_t channel)
{
    uint8_t inpmux;
    uint8_t readback = 0;
    ads1263_status_t status;

    if (dev == NULL) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    if (channel != 0U) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    inpmux = ADS1263_INPMUX_AIN0_AINCOM;
    status = ads1263_write_registers(dev, ADS1263_REG_INPMUX, 1U, &inpmux);
    if (status != ADS1263_OK) {
        return status;
    }

    status = ads1263_read_registers(dev, ADS1263_REG_INPMUX, 1U, &readback);
    if (status != ADS1263_OK) {
        return status;
    }

    if (readback != inpmux) {
        return ADS1263_ERROR_REGISTER_MISMATCH;
    }

    return ads1263_send_command(dev, ADS1263_CMD_START1);
}

ads1263_status_t ads1263_read_drdy(ads1263_t *dev, int *value)
{
    if (dev == NULL || value == NULL) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    if (dev->config.use_drdy == 0) {
        *value = (dev->config.mock != 0) ? 1 : -1;
        return ADS1263_OK;
    }

    return ads1263_gpio_linux_read_drdy(&dev->gpio, value);
}

static ads1263_status_t ads1263_wait_drdy(ads1263_t *dev, int *last_value)
{
    const unsigned int timeout_ms = 1000U;
    const struct timespec sleep_time = { 0, 1000000L };

    if (dev == NULL || last_value == NULL) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    if (dev->config.use_drdy == 0) {
        *last_value = (dev->config.mock != 0) ? 1 : -1;
        return ADS1263_OK;
    }

    for (unsigned int elapsed = 0; elapsed < timeout_ms; ++elapsed) {
        ads1263_status_t status = ads1263_read_drdy(dev, last_value);
        if (status != ADS1263_OK) {
            return status;
        }

        if (*last_value == 0) {
            return ADS1263_OK;
        }

        (void)nanosleep(&sleep_time, NULL);
    }

    return ADS1263_ERROR_TIMEOUT;
}

static int32_t parse_signed_32(const uint8_t *bytes)
{
    uint32_t raw_u32;

    raw_u32 = ((uint32_t)bytes[0] << 24U) |
              ((uint32_t)bytes[1] << 16U) |
              ((uint32_t)bytes[2] << 8U) |
              ((uint32_t)bytes[3]);

    return (int32_t)raw_u32;
}

ads1263_status_t ads1263_read_sample(ads1263_t *dev, ads1263_sample_t *sample)
{
    uint8_t tx[7] = { ADS1263_CMD_RDATA1, 0, 0, 0, 0, 0, 0 };
    uint8_t rx[7] = { 0 };
    ads1263_status_t drdy_status;
    ads1263_status_t status;
    int drdy = -1;

    if (dev == NULL || sample == NULL) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    memset(sample, 0, sizeof(*sample));

    if (dev->config.mock != 0) {
        dev->mock_sample_counter++;
        sample->raw = (int32_t)((dev->mock_sample_counter * 1234567U) % 2000000000U);
        sample->voltage = ads1263_raw_to_voltage(sample->raw, dev->config.vref, dev->config.gain);
        sample->drdy = 1;
        sample->status_byte = 0x80U;
        sample->checksum_byte = 0x00U;
        sample->status = "OK";
        return ADS1263_OK;
    }

    drdy_status = ads1263_wait_drdy(dev, &drdy);

    status = ads1263_spi_linux_transfer(&dev->spi, tx, rx, sizeof(tx));
    if (status != ADS1263_OK) {
        return status;
    }

    sample->status_byte = rx[1];
    sample->raw = parse_signed_32(&rx[2]);
    sample->checksum_byte = rx[6];
    sample->voltage = ads1263_raw_to_voltage(sample->raw, dev->config.vref, dev->config.gain);
    sample->drdy = drdy;
    sample->status = (drdy_status == ADS1263_OK) ? "OK" : ads1263_status_to_string(drdy_status);

    return ADS1263_OK;
}

double ads1263_raw_to_voltage(int32_t raw, double vref, uint8_t gain)
{
    double effective_gain;

    effective_gain = (gain == 0U) ? 1.0 : (double)gain;
    return ((double)raw / ADS1263_ADC1_POSITIVE_FULL_SCALE) * (vref / effective_gain);
}

const char *ads1263_status_to_string(ads1263_status_t status)
{
    switch (status) {
    case ADS1263_OK:
        return "OK";
    case ADS1263_ERROR_SPI_OPEN:
        return "SPI_OPEN";
    case ADS1263_ERROR_SPI_CONFIG:
        return "SPI_CONFIG";
    case ADS1263_ERROR_SPI_TRANSFER:
        return "SPI_TRANSFER";
    case ADS1263_ERROR_GPIO_OPEN:
        return "GPIO_OPEN";
    case ADS1263_ERROR_GPIO_READ:
        return "GPIO_READ";
    case ADS1263_ERROR_NOT_DETECTED:
        return "NOT_DETECTED";
    case ADS1263_ERROR_NOT_DETECTED_ZERO:
        return "NOT_DETECTED_ZERO";
    case ADS1263_ERROR_NOT_DETECTED_FLOATING:
        return "NOT_DETECTED_FLOATING";
    case ADS1263_ERROR_REGISTER_MISMATCH:
        return "REGISTER_MISMATCH";
    case ADS1263_ERROR_INVALID_ARG:
        return "INVALID_ARG";
    case ADS1263_ERROR_TIMEOUT:
        return "DRDY_TIMEOUT";
    case ADS1263_ERROR_IO:
        return "IO";
    default:
        return "UNKNOWN";
    }
}
