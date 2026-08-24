# ADS1263 component design

The ADS1263 component exposes ADC-oriented operations and keeps Linux details outside the generic API.

## Public behavior

- `ads1263_check_presence()` reads registers `ID..MODE1`.
- `ads1263_configure_channel()` currently supports only channel `0`.
- Channel `0` writes `INPMUX = 0x0A`, which maps `AIN0` as positive input and `AINCOM` as negative input.
- `ads1263_read_sample()` reads ADC1 conversion data with `RDATA1`.

## Register check

The validated SPI transaction is:

```text
TX: 20 04 00 00 00 00 00
RX: 00 00 23 11 05 00 80
```

The first two received bytes correspond to command latency/dummy bytes. Register values begin at `rx[2]`.

Presence validation intentionally does not hard-fail on `ID != 0x23`, because the ADS1263 revision bits may vary. It does fail when all registers read as `0x00`, all registers read as `0xFF`, or the known baseline registers differ from:

```text
POWER     = 0x11
INTERFACE = 0x05
MODE0     = 0x00
MODE1     = 0x80
```

## Sample format assumption

The reset `INTERFACE` value is `0x05`, which enables a status byte and checksum mode in the ADS1263 default data sequence. For this demo, `RDATA1` is read as:

```text
command byte + status byte + 4 data bytes + checksum byte
```

The code stores the status and checksum bytes in `ads1263_sample_t`, but only the signed 32-bit ADC data is converted to voltage.

## Voltage estimate

The voltage is approximate:

```text
voltage = raw / 2147483647 * (vref / gain)
```

The default is `vref = 2.5 V` and `gain = 1`.
