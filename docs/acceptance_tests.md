# Acceptance tests

## Host build

```bash
source /home/zeus/sdk/poky-stm32mp1-ads1263/5.0.18/environment-setup-cortexa7t2hf-neon-vfpv4-poky-linux-gnueabi
make clean
make
file build/ads1263_app
```

Pass criteria:

- `make` succeeds.
- `file build/ads1263_app` reports ARM 32-bit ELF.

## Target setup

```bash
ls -l /dev/spidev0.0 || ./scripts/bind_spidev.sh
gpioinfo -c gpiochip6
```

Pass criteria:

- `/dev/spidev0.0` exists.
- `gpiochip6` line 8 is visible as `PG8`.

## Software-only target validation

Use this when the ADS1263 and potentiometer are not connected yet:

```bash
./ads1263_app --check --mock
./ads1263_app --samples 5 --channel 0 --rate-hz 2 --csv /tmp/ads1263_mock.csv --mock
cat /tmp/ads1263_mock.csv
```

Pass criteria:

- `--check --mock` exits with code `0`.
- Mock register values match the ADS1263 validated baseline.
- Exactly 5 sample lines are printed.
- `/tmp/ads1263_mock.csv` contains one header row and 5 data rows.

## Check connected ADS1263

```bash
./ads1263_app --check
```

Pass criteria:

- Register snapshot is printed.
- `POWER = 0x11`.
- `INTERFACE = 0x05`.
- `MODE0 = 0x00`.
- `MODE1 = 0x80`.
- Output includes `ADS1263 detected`.

## Check disconnected ADS1263

Disconnect SPI or power from ADS1263, then run:

```bash
./ads1263_app --check
```

Pass criteria:

- Command exits non-zero.
- Output clearly reports not detected.

## Invalid parameters

```bash
./ads1263_app --samples 0 --rate-hz 50
```

Pass criteria:

- Command exits non-zero.
- Output reports invalid `samples` and invalid `rate-hz`.

## Fixed samples

```bash
./ads1263_app --samples 10 --channel 0 --rate-hz 2
```

Pass criteria:

- Exactly 10 lines beginning with `sample=` are printed.
- Turning the potentiometer changes raw and voltage values.

## Live mode

```bash
./ads1263_app --live --channel 0 --rate-hz 2
```

Pass criteria:

- Samples continue until Ctrl+C.
- Process exits cleanly after Ctrl+C.

## CSV

```bash
./ads1263_app --samples 20 --channel 0 --rate-hz 5 --csv /tmp/ads1263_demo.csv
cat /tmp/ads1263_demo.csv
```

Pass criteria:

- CSV exists.
- Header is `sample,raw,voltage,status`.
- File contains 20 data rows.
