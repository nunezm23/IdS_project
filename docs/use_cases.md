# Use cases

## UC-001: Check ADS1263 presence

Primary actor: developer on STM32 target.

Command:

```bash
./ads1263_app --check
```

Main success path:

1. Open `/dev/spidev0.0`.
2. Configure SPI mode 1, 8 bits and 1 MHz.
3. Open `/dev/gpiochip6` line 8.
4. Read ADS1263 registers `0x00..0x04`.
5. Print register snapshot.
6. Print `ADS1263 detected`.

Failure path:

- If all registers are `0x00`, report wiring/power/SPI issue.
- If all registers are `0xFF`, report floating MISO/CS/power issue.
- If baseline registers differ, report register mismatch.

## UC-002: Acquire fixed samples

Command:

```bash
./ads1263_app --samples 10 --channel 0 --rate-hz 2
```

Expected result: exactly 10 sample lines with raw value, approximate voltage, DRDY state and status.

## UC-003: Acquire live samples

Command:

```bash
./ads1263_app --live --channel 0 --rate-hz 2
```

Expected result: continuous sample lines until Ctrl+C.

## UC-004: Save CSV

Command:

```bash
./ads1263_app --samples 20 --channel 0 --rate-hz 5 --csv /tmp/ads1263_demo.csv
```

Expected result: console output plus a CSV file with header:

```csv
sample,raw,voltage,status
```
