# Requirements

| ID | Requirement | Verification |
| --- | --- | --- |
| REQ-001 | The application shall run in Linux user space on STM32MP157D-DK1. | Build and execute `ads1263_app` on target. |
| REQ-002 | The application shall communicate with ADS1263 through `/dev/spidev0.0`. | `--check` opens SPI device and reads registers. |
| REQ-003 | SPI shall use mode 1, 8 bits per word and 1 MHz by default. | Code inspection and `spidev` configuration path. |
| REQ-004 | The application shall read ADS1263 registers `0x00..0x04`. | `--check` output. |
| REQ-005 | The application shall detect disconnected SPI when all registers read `0x00` or `0xFF`. | Disconnect/power-off ADS1263 and run `--check`. |
| REQ-006 | The application shall read DRDY from `/dev/gpiochip6` line `8`. | `--check` prints DRDY value. |
| REQ-007 | The application shall acquire samples from channel `0`. | Run `--samples N --channel 0 --rate-hz HZ`. |
| REQ-008 | The application shall support fixed sample count mode. | Verify exactly `N` sample lines. |
| REQ-009 | The application shall support live mode. | Run `--live` and stop with Ctrl+C. |
| REQ-010 | The application shall optionally write CSV output. | Run with `--csv /tmp/ads1263_demo.csv`. |
| REQ-011 | Invalid parameters shall be rejected with clear errors. | Run invalid CLI values. |
| REQ-012 | Linux-specific code shall remain isolated in `components/ads1263/arch/linux`. | Code inspection. |
