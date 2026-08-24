#include "ads1263_spi_linux.h"

#include <errno.h>
#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

ads1263_status_t ads1263_spi_linux_open(ads1263_spi_linux_t *spi,
                                        const char *device,
                                        uint32_t speed_hz)
{
    uint8_t lsb_first = 0;

    if (spi == NULL || device == NULL) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    memset(spi, 0, sizeof(*spi));
    spi->fd = -1;
    spi->speed_hz = speed_hz;
    spi->mode = SPI_MODE_1;
    spi->bits_per_word = 8;

    spi->fd = open(device, O_RDWR);
    if (spi->fd < 0) {
        return ADS1263_ERROR_SPI_OPEN;
    }

    if (ioctl(spi->fd, SPI_IOC_WR_MODE, &spi->mode) < 0 ||
        ioctl(spi->fd, SPI_IOC_WR_BITS_PER_WORD, &spi->bits_per_word) < 0 ||
        ioctl(spi->fd, SPI_IOC_WR_MAX_SPEED_HZ, &spi->speed_hz) < 0 ||
        ioctl(spi->fd, SPI_IOC_WR_LSB_FIRST, &lsb_first) < 0) {
        ads1263_spi_linux_close(spi);
        return ADS1263_ERROR_SPI_CONFIG;
    }

    return ADS1263_OK;
}

ads1263_status_t ads1263_spi_linux_transfer(ads1263_spi_linux_t *spi,
                                            const uint8_t *tx,
                                            uint8_t *rx,
                                            size_t len)
{
    struct spi_ioc_transfer transfer;
    int ret;

    if (spi == NULL || spi->fd < 0 || tx == NULL || rx == NULL || len == 0U) {
        return ADS1263_ERROR_INVALID_ARG;
    }

    memset(&transfer, 0, sizeof(transfer));
    transfer.tx_buf = (uintptr_t)tx;
    transfer.rx_buf = (uintptr_t)rx;
    transfer.len = (uint32_t)len;
    transfer.speed_hz = spi->speed_hz;
    transfer.bits_per_word = spi->bits_per_word;

    ret = ioctl(spi->fd, SPI_IOC_MESSAGE(1), &transfer);
    if (ret < 0 || (size_t)ret != len) {
        return ADS1263_ERROR_SPI_TRANSFER;
    }

    return ADS1263_OK;
}

void ads1263_spi_linux_close(ads1263_spi_linux_t *spi)
{
    if (spi == NULL) {
        return;
    }

    if (spi->fd >= 0) {
        (void)close(spi->fd);
    }

    spi->fd = -1;
}
