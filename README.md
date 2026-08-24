# ADS1263 Data Logger for STM32MP157D-DK1

User-space C demo for reading a potentiometer through a Waveshare ADS1263 module on an STM32MP157D-DK1 running Yocto/OpenSTLinux.

The project is intentionally modular: the application flow lives in `components/acquisition`, ADS1263 protocol logic lives in `components/ads1263`, and Linux-only SPI/GPIO code is isolated under `components/ads1263/arch/linux`.

## Validated hardware baseline

| Signal | ADS1263 / Waveshare | STM32MP157D-DK1 |
| --- | --- | --- |
| SCK | SCK | PE12 / D13 / SPI4_SCK |
| MISO | DOUT | PE13 / D12 / SPI4_MISO |
| MOSI | DIN | PE14 / D11 / SPI4_MOSI |
| CS | CS | PE11 / D10 / SPI4_NSS |
| DRDY | DRDY | PG8, `/dev/gpiochip6` line `8` |
| GND | GND | GND |

Runtime defaults:

- SPI device: `/dev/spidev0.0`
- SPI mode: `1`
- SPI speed: `1000000` Hz
- GPIO chip: `/dev/gpiochip6`
- DRDY line: `8`
- ADC input: `AIN0` versus `AINCOM`
- Vref assumption for voltage estimate: `2.5 V`
- ADC gain assumption: `1`

## Build on the Ubuntu host with the Yocto SDK

```bash
cd ads1263-data-logger
source /home/zeus/sdk/poky-stm32mp1-ads1263/5.0.18/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi
make clean
make
file build/ads1263_app
```

The expected `file` output is an ARM 32-bit ELF binary.

## Deploy to the STM32MP157D-DK1

```bash
make deploy TARGET=root@192.168.0.9
```

Equivalent manual command:

```bash
scp build/ads1263_app root@192.168.0.9:~/
```

## Prepare `/dev/spidev0.0` on target if needed

If `/dev/spidev0.0` is missing after boot, run this on the STM32 target:

```bash
modprobe spidev 2>/dev/null || true
echo spidev > /sys/bus/spi/devices/spi0.0/driver_override
echo spi0.0 > /sys/bus/spi/drivers/spidev/bind
ls -l /dev/spidev0.0
```

The helper script contains the same commands:

```bash
./scripts/bind_spidev.sh
```

## Run on target

```bash
chmod +x ~/ads1263_app
~/ads1263_app --help
~/ads1263_app --check
~/ads1263_app --samples 10 --channel 0 --rate-hz 2
~/ads1263_app --live --channel 0 --rate-hz 2
~/ads1263_app --samples 20 --channel 0 --rate-hz 5 --csv /tmp/ads1263_demo.csv
cat /tmp/ads1263_demo.csv
```

## Expected check output

Connected ADS1263:

```text
[INFO] ADS1263 register check
ID        = 0x23
POWER     = 0x11
INTERFACE = 0x05
MODE0     = 0x00
MODE1     = 0x80
[INFO] ADS1263 detected
```

Disconnected SPI or unpowered ADS1263:

```text
[ERROR] ADS1263 not detected: all registers read as 0x00
[ERROR] Check SPI wiring, ADS1263 power, GND, CS, MISO, MOSI, SCLK and SPI mode.
```

## Notes

- This is a user-space demo, not a kernel driver and not an IIO driver.
- `--check` reads ADS1263 registers `0x00` through `0x04` using the validated RREG transfer.
- `--samples` and `--live` configure `INPMUX` for `AIN0-AINCOM`, send `START1`, poll/read DRDY, then read ADC1 data with `RDATA1`.
- Voltage is an approximation using `raw / 2147483647 * (vref / gain)`.
- RESET is intentionally not controlled because PI5 was observed as owned by the kernel.

## Optional local syntax build without libgpiod

This is only for host-side syntax checks in environments that do not have libgpiod headers installed:

```bash
make clean
make NO_GPIOD=1
```

Do not use `NO_GPIOD=1` for the STM32 target demo.

## Software-only validation without ADS1263 connected

Use `--mock` when the ADS1263 and potentiometer are not physically connected yet. This validates the application-level flow without opening SPI or GPIO:

```bash
./ads1263_app --check --mock
./ads1263_app --samples 5 --channel 0 --rate-hz 2 --csv /tmp/ads1263_mock.csv --mock
cat /tmp/ads1263_mock.csv
./ads1263_app --live --channel 0 --rate-hz 2 --mock
```

Expected behavior:

- `--check --mock` prints `ID=0x23`, `POWER=0x11`, `INTERFACE=0x05`, `MODE0=0x00`, `MODE1=0x80`.
- `--samples ... --mock` prints exactly the requested number of samples.
- `--csv ... --mock` creates a CSV file with a header and sample rows.
- `--live ... --mock` prints continuous simulated samples until Ctrl+C.

This mode does not prove the physical SPI/DRDY connection. It proves that CLI parsing, acquisition orchestration, register validation, sample formatting, CSV output and target runtime are working.
