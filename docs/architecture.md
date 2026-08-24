# Architecture

The demo uses process segmentation at code level: CLI, use-case controller, device component, Linux HAL and logging are separate.

## Components

| Component | Responsibility |
| --- | --- |
| `main` | CLI parsing and command selection. |
| `acquisition` | Use-case flow for check, fixed samples and live acquisition. |
| `ads1263` | ADS1263 commands, register validation, channel setup and sample conversion. |
| `ads1263/arch/linux` | Linux `spidev` and `libgpiod` access. |
| `logger` | Console and CSV formatting. |

## Runtime flow

1. Parse command-line options.
2. Build `ads1263_config_t` and `acquisition_config_t`.
3. Open SPI and DRDY GPIO.
4. Validate ADS1263 presence with RREG.
5. For acquisition modes, write `INPMUX` for `AIN0-AINCOM` and send `START1`.
6. Read samples with `RDATA1`.
7. Print console rows and optionally append CSV rows.
8. Release GPIO/SPI resources.

## Error strategy

The code returns explicit status codes. User-facing messages are produced in the acquisition/logger layers so low-level drivers remain reusable.
