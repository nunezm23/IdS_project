#!/bin/sh
set -eu

modprobe spidev 2>/dev/null || true

if [ ! -d /sys/bus/spi/devices/spi0.0 ]; then
    echo "[ERROR] /sys/bus/spi/devices/spi0.0 not found"
    echo "[ERROR] Check Device Tree/SPI binding for the STM32MP157D-DK1 ADS1263 node"
    exit 1
fi

echo spidev > /sys/bus/spi/devices/spi0.0/driver_override

if [ -e /sys/bus/spi/drivers/spidev/bind ]; then
    echo spi0.0 > /sys/bus/spi/drivers/spidev/bind 2>/dev/null || true
fi

ls -l /dev/spidev0.0
