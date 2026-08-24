#ifndef ADS1263_SPI_LINUX_H
#define ADS1263_SPI_LINUX_H

#include "ads1263_defs.h"

#include <stddef.h>
#include <stdint.h>

typedef struct {
    int fd;
    uint32_t speed_hz;
    uint8_t mode;
    uint8_t bits_per_word;
} ads1263_spi_linux_t;

/**
 * @brief Open and configure a Linux spidev device for ADS1263 transfers.
 */
ads1263_status_t ads1263_spi_linux_open(ads1263_spi_linux_t *spi,
                                        const char *device,
                                        uint32_t speed_hz);

/**
 * @brief Run a full-duplex SPI transaction with SPI_IOC_MESSAGE(1).
 */
ads1263_status_t ads1263_spi_linux_transfer(ads1263_spi_linux_t *spi,
                                            const uint8_t *tx,
                                            uint8_t *rx,
                                            size_t len);

/**
 * @brief Close a Linux spidev file descriptor.
 */
void ads1263_spi_linux_close(ads1263_spi_linux_t *spi);

#endif /* ADS1263_SPI_LINUX_H */
