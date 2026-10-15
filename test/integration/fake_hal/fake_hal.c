#include "ads1263_gpio_linux.h"
#include "ads1263_spi_linux.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int transfer_index;
static unsigned int gpio_read_index;

static const char *scenario(void)
{
    const char *value = getenv("FAKE_HAL_SCENARIO");

    return value != NULL ? value : "";
}

static int scenario_is(const char *expected)
{
    return strcmp(scenario(), expected) == 0;
}

static int supported_scenario(void)
{
    return scenario_is("happy_sample") ||
           scenario_is("delayed_drdy") ||
           scenario_is("read_failure") ||
           scenario_is("presence_failure");
}

static void trace_event(const char *format, ...)
{
    const char *path = getenv("FAKE_HAL_TRACE");
    FILE *stream;
    va_list args;

    if (path == NULL || path[0] == '\0') {
        return;
    }

    stream = fopen(path, "a");
    if (stream == NULL) {
        return;
    }

    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
    fputc('\n', stream);
    fclose(stream);
}

static int frame_equals(const uint8_t *tx, size_t len, const uint8_t *expected, size_t expected_len)
{
    return len == expected_len && memcmp(tx, expected, len) == 0;
}

static void trace_transfer(const uint8_t *tx, size_t len, const char *result)
{
    const char hex[] = "0123456789ABCDEF";
    char frame[3U * 34U];
    size_t position = 0;

    for (size_t i = 0; i < len && i < 34U; ++i) {
        if (i != 0U) {
            frame[position++] = ':';
        }
        frame[position++] = hex[(tx[i] >> 4U) & 0x0FU];
        frame[position++] = hex[tx[i] & 0x0FU];
    }
    frame[position] = '\0';
    trace_event("SPI_TRANSFER len=%zu tx=%s result=%s", len, frame, result);
}

ads1263_status_t ads1263_spi_linux_open(ads1263_spi_linux_t *spi,
                                        const char *device,
                                        uint32_t speed_hz)
{
    if (spi == NULL || device == NULL || !supported_scenario()) {
        trace_event("SPI_OPEN result=INVALID_ARG");
        return ADS1263_ERROR_INVALID_ARG;
    }

    memset(spi, 0, sizeof(*spi));
    spi->fd = 101;
    spi->speed_hz = speed_hz;
    spi->mode = 1U;
    spi->bits_per_word = 8U;
    transfer_index = 0U;
    gpio_read_index = 0U;
    trace_event("SPI_OPEN device=%s speed=%u result=OK", device, speed_hz);
    return ADS1263_OK;
}

ads1263_status_t ads1263_spi_linux_transfer(ads1263_spi_linux_t *spi,
                                            const uint8_t *tx,
                                            uint8_t *rx,
                                            size_t len)
{
    static const uint8_t presence[] = { 0x20U, 0x04U, 0, 0, 0, 0, 0 };
    static const uint8_t write_mux[] = { 0x46U, 0x00U, 0x0AU };
    static const uint8_t read_mux[] = { 0x26U, 0x00U, 0x00U };
    static const uint8_t start[] = { 0x08U };
    static const uint8_t read_data[] = { 0x12U, 0, 0, 0, 0, 0, 0 };
    int valid = 0;

    if (spi == NULL || spi->fd != 101 || tx == NULL || rx == NULL || len == 0U) {
        trace_event("SPI_TRANSFER result=INVALID_ARG");
        return ADS1263_ERROR_INVALID_ARG;
    }

    memset(rx, 0, len);
    switch (transfer_index) {
    case 0:
        valid = frame_equals(tx, len, presence, sizeof(presence));
        if (valid && !scenario_is("presence_failure")) {
            rx[2] = 0x23U;
            rx[3] = ADS1263_DEFAULT_POWER;
            rx[4] = ADS1263_DEFAULT_INTERFACE;
            rx[5] = ADS1263_DEFAULT_MODE0;
            rx[6] = ADS1263_DEFAULT_MODE1;
        }
        break;
    case 1:
        valid = !scenario_is("presence_failure") &&
                frame_equals(tx, len, write_mux, sizeof(write_mux));
        break;
    case 2:
        valid = frame_equals(tx, len, read_mux, sizeof(read_mux));
        if (valid) {
            rx[2] = 0x0AU;
        }
        break;
    case 3:
        valid = frame_equals(tx, len, start, sizeof(start));
        break;
    case 4:
        valid = frame_equals(tx, len, read_data, sizeof(read_data));
        if (valid && !scenario_is("read_failure")) {
            rx[1] = 0x80U;
            rx[2] = 0x00U;
            rx[3] = 0x12U;
            rx[4] = 0xD6U;
            rx[5] = 0x87U;
            rx[6] = 0x5AU;
        }
        break;
    default:
        valid = 0;
        break;
    }

    transfer_index++;
    if (!valid) {
        trace_transfer(tx, len, "UNEXPECTED");
        return ADS1263_ERROR_SPI_TRANSFER;
    }
    if (transfer_index == 5U && scenario_is("read_failure")) {
        trace_transfer(tx, len, "SPI_TRANSFER");
        return ADS1263_ERROR_SPI_TRANSFER;
    }

    trace_transfer(tx, len, "OK");
    return ADS1263_OK;
}

void ads1263_spi_linux_close(ads1263_spi_linux_t *spi)
{
    if (spi != NULL) {
        spi->fd = -1;
    }
    trace_event("SPI_CLOSE");
}

ads1263_status_t ads1263_gpio_linux_open(ads1263_gpio_linux_t *gpio,
                                         const char *chip_path,
                                         unsigned int line)
{
    if (gpio == NULL || chip_path == NULL || !supported_scenario()) {
        trace_event("GPIO_OPEN result=INVALID_ARG");
        return ADS1263_ERROR_INVALID_ARG;
    }

    memset(gpio, 0, sizeof(*gpio));
    gpio->line = line;
    gpio->enabled = 1;
    trace_event("GPIO_OPEN chip=%s line=%u result=OK", chip_path, line);
    return ADS1263_OK;
}

ads1263_status_t ads1263_gpio_linux_read_drdy(ads1263_gpio_linux_t *gpio, int *value)
{
    if (gpio == NULL || value == NULL || gpio->enabled == 0) {
        trace_event("GPIO_READ result=INVALID_ARG");
        return ADS1263_ERROR_INVALID_ARG;
    }

    if (scenario_is("delayed_drdy") && gpio_read_index < 2U) {
        *value = 1;
    } else {
        *value = 0;
    }
    gpio_read_index++;
    trace_event("GPIO_READ value=%d result=OK", *value);
    return ADS1263_OK;
}

void ads1263_gpio_linux_close(ads1263_gpio_linux_t *gpio)
{
    if (gpio != NULL) {
        gpio->enabled = 0;
        gpio->chip = NULL;
        gpio->request = NULL;
    }
    trace_event("GPIO_CLOSE");
}
