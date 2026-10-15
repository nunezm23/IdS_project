# Static-analysis findings and limitations

## Reproduction key

| Key | Artifact |
|---|---|
| `CP-R1` | `raw_analysis/001_cppcheck_no_gpiod.command.txt` and adjacent raw output/exit/time files |
| `CP-R2` | `raw_analysis_exhaustive/001_cppcheck_exhaustive_no_gpiod.command.txt` and adjacent raw output/exit/time files |
| `GCC-R1` | `raw_analysis/003_gcc15_no_gpiod.command.txt` and adjacent raw output/exit/time files |
| `GATE` | `raw_metadata/015_*`, `016_*`, `017_*`, `018_*`, `019_*` |

`REPRODUCED` means the diagnostic appeared in both `CP-R1` and `CP-R2`. All rows apply to HEAD `54a91ed4b684b5ac6f4221c8743768fad5083270` and the source hashes in `raw_metadata/021_source_sha256.stdout.txt`.

## Disposition and risk records

| Link | Disposition |
|---|---|
| `AR-SA-001` | Accepted analysis limitation: Cppcheck does not infer that Linux `ioctl(SPI_IOC_MESSAGE)` writes through the integer-valued `rx_buf` address. The public transfer contract correctly requires writable `uint8_t *rx`; changing it to const would be wrong. GCC 15 reports no diagnostic. |
| `AR-SA-002` | Accepted analysis limitation: direct analysis of all six headers, required by scope, treats each header as a standalone primary input and produces `unusedStructMember` diagnostics even though the data model is consumed by production C files or the alternate GPIO configuration. No member is removed based on this artifact. |
| `AR-SA-003` | Accepted low maintainability risk/interface decision: three functions have no caller outside `ads1263.c` in the six-file production call graph, but are deliberately declared public in `ads1263.h` and are selected external unit-test/API surfaces. They remain non-static. |
| `AR-SA-004` | Accepted Cppcheck environment limitation: Cppcheck emits 32 `missingIncludeSystem` notices. The headers are sufficient for GCC 15 no-gpiod syntax analysis, but Cppcheck's system-header model remains limited. No suppression hides the notices. |
| `AR-SA-005` | Closed analysis-depth limitation: the required default-level run limited branches in two files. `CP-R2` added `--check-level=exhaustive`, completed all 12 files, and did not repeat either notice. |
| `BLK-SA-001` | Open environment blocker: libgpiod-v2 analysis cannot run because `pkg-config` is unavailable and `gpiod.h` is absent. Only `ADS1263_NO_GPIOD=0` is blocked. No package installation was attempted. |

## Cppcheck source findings

These are actual analyzer source diagnostics, not confirmed defects. Tool severity is `style` for all 41. Impact is assessed independently below.

| Stable ID | Source location / Cppcheck ID | Severity and impact | Reproducibility | Disposition | Defect / accepted-risk link |
|---|---|---|---|---|---|
| `SA-CPPCHECK-001` | `components/ads1263/arch/linux/src/ads1263_spi_linux.c:46:54`, `constParameterPointer` | Style; no production impact | `REPRODUCED` by `CP-R1`, `CP-R2` | False positive: `rx` is written by the kernel through `spi_ioc_transfer.rx_buf` at lines 58 and 63 | No defect; `AR-SA-001` |
| `SA-CPPCHECK-002` | `components/acquisition/include/acquisition.h:9:13`, `unusedStructMember`, `channel` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-003` | `components/acquisition/include/acquisition.h:10:14`, `unusedStructMember`, `samples` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-004` | `components/acquisition/include/acquisition.h:11:14`, `unusedStructMember`, `rate_hz` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-005` | `components/acquisition/include/acquisition.h:12:9`, `unusedStructMember`, `live` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-006` | `components/acquisition/include/acquisition.h:13:17`, `unusedStructMember`, `csv_path` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-007` | `components/ads1263/include/ads1263.h:12:22`, `unusedStructMember`, `config` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-008` | `components/ads1263/include/ads1263.h:13:25`, `unusedStructMember`, `spi` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-009` | `components/ads1263/include/ads1263.h:15:9`, `unusedStructMember`, `initialized` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-010` | `components/ads1263/include/ads1263.h:16:13`, `unusedStructMember`, `mock_registers` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-011` | `components/ads1263/include/ads1263.h:17:14`, `unusedStructMember`, `mock_sample_counter` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-012` | `components/ads1263/include/ads1263_defs.h:57:13`, `unusedStructMember`, `raw` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-013` | `components/ads1263/include/ads1263_defs.h:58:12`, `unusedStructMember`, `voltage` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-014` | `components/ads1263/include/ads1263_defs.h:59:9`, `unusedStructMember`, `drdy` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-015` | `components/ads1263/include/ads1263_defs.h:60:13`, `unusedStructMember`, `status_byte` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-016` | `components/ads1263/include/ads1263_defs.h:61:13`, `unusedStructMember`, `checksum_byte` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-017` | `components/ads1263/include/ads1263_defs.h:62:17`, `unusedStructMember`, `status` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-018` | `components/ads1263/include/ads1263_defs.h:66:13`, `unusedStructMember`, `id` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-019` | `components/ads1263/include/ads1263_defs.h:67:13`, `unusedStructMember`, `power` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-020` | `components/ads1263/include/ads1263_defs.h:68:13`, `unusedStructMember`, `interface` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-021` | `components/ads1263/include/ads1263_defs.h:69:13`, `unusedStructMember`, `mode0` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-022` | `components/ads1263/include/ads1263_defs.h:70:13`, `unusedStructMember`, `mode1` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-023` | `components/ads1263/include/ads1263_defs.h:74:17`, `unusedStructMember`, `spi_device` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-024` | `components/ads1263/include/ads1263_defs.h:75:17`, `unusedStructMember`, `gpio_chip` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-025` | `components/ads1263/include/ads1263_defs.h:76:18`, `unusedStructMember`, `drdy_line` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-026` | `components/ads1263/include/ads1263_defs.h:77:14`, `unusedStructMember`, `spi_speed_hz` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-027` | `components/ads1263/include/ads1263_defs.h:78:12`, `unusedStructMember`, `vref` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-028` | `components/ads1263/include/ads1263_defs.h:79:13`, `unusedStructMember`, `gain` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-029` | `components/ads1263/include/ads1263_defs.h:80:9`, `unusedStructMember`, `use_drdy` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-030` | `components/ads1263/include/ads1263_defs.h:81:9`, `unusedStructMember`, `mock` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-031` | `components/ads1263/arch/linux/include/ads1263_spi_linux.h:10:9`, `unusedStructMember`, `fd` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-032` | `components/ads1263/arch/linux/include/ads1263_spi_linux.h:11:14`, `unusedStructMember`, `speed_hz` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-033` | `components/ads1263/arch/linux/include/ads1263_spi_linux.h:12:13`, `unusedStructMember`, `mode` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-034` | `components/ads1263/arch/linux/include/ads1263_spi_linux.h:13:13`, `unusedStructMember`, `bits_per_word` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive | No defect; `AR-SA-002` |
| `SA-CPPCHECK-035` | `components/ads1263/arch/linux/include/ads1263_gpio_linux.h:10:24`, `unusedStructMember`, `chip` | Style; no demonstrated no-gpiod runtime impact | `REPRODUCED` | Header/configuration-context observation; member belongs to blocked real-gpiod branch | No defect; `AR-SA-002`, `BLK-SA-001` |
| `SA-CPPCHECK-036` | `components/ads1263/arch/linux/include/ads1263_gpio_linux.h:11:32`, `unusedStructMember`, `request` | Style; no demonstrated no-gpiod runtime impact | `REPRODUCED` | Header/configuration-context observation; member belongs to blocked real-gpiod branch | No defect; `AR-SA-002`, `BLK-SA-001` |
| `SA-CPPCHECK-037` | `components/ads1263/arch/linux/include/ads1263_gpio_linux.h:12:18`, `unusedStructMember`, `line` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive; no-gpiod source uses the member | No defect; `AR-SA-002` |
| `SA-CPPCHECK-038` | `components/ads1263/arch/linux/include/ads1263_gpio_linux.h:13:9`, `unusedStructMember`, `enabled` | Style; no demonstrated runtime impact | `REPRODUCED` | Standalone-header context false positive; no-gpiod source uses the member | No defect; `AR-SA-002` |
| `SA-CPPCHECK-039` | `components/ads1263/src/ads1263.c:92:18`, `staticFunction`, `ads1263_read_registers` | Style; low interface-surface maintainability impact | `REPRODUCED` | Accepted public API design; declared in `ads1263.h` and selected as an external SUT | No defect; `AR-SA-003` |
| `SA-CPPCHECK-040` | `components/ads1263/src/ads1263.c:127:18`, `staticFunction`, `ads1263_write_registers` | Style; low interface-surface maintainability impact | `REPRODUCED` | Accepted public API design; declared in `ads1263.h` and selected as an external SUT | No defect; `AR-SA-003` |
| `SA-CPPCHECK-041` | `components/ads1263/src/ads1263.c:355:8`, `staticFunction`, `ads1263_raw_to_voltage` | Style; low interface-surface maintainability impact | `REPRODUCED` | Accepted public API design; declared in `ads1263.h` and selected as an external SUT | No defect; `AR-SA-003` |

## Environment and completeness records

| Stable ID | Source/configuration location | Severity and impact | Reproducibility | Disposition | Defect / accepted-risk link |
|---|---|---|---|---|---|
| `SA-ENV-001` | `components/ads1263/include/ads1263_defs.h:4:2`, `<stdint.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-002` | `components/acquisition/include/acquisition.h:6:2`, `<stdint.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-003` | `components/ads1263/arch/linux/include/ads1263_spi_linux.h:6:2`, `<stddef.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-004` | `components/ads1263/arch/linux/include/ads1263_spi_linux.h:7:2`, `<stdint.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-005` | `components/ads1263/include/ads1263.h:8:2`, `<stddef.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-006` | `components/ads1263/include/ads1263.h:9:2`, `<stdint.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-007` | `components/logger/include/app_log.h:6:2`, `<stdint.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-008` | `components/logger/include/app_log.h:7:2`, `<stdio.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-009` | `main/main.c:5:2`, `<errno.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-010` | `main/main.c:6:2`, `<stdint.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-011` | `main/main.c:7:2`, `<stdio.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-012` | `main/main.c:8:2`, `<stdlib.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-013` | `main/main.c:9:2`, `<string.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-014` | `components/logger/src/app_log.c:3:2`, `<inttypes.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-015` | `components/logger/src/app_log.c:4:2`, `<stdarg.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-016` | `components/logger/src/app_log.c:5:2`, `<stdio.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-017` | `components/acquisition/src/acquisition.c:6:2`, `<signal.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-018` | `components/acquisition/src/acquisition.c:7:2`, `<stdint.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-019` | `components/acquisition/src/acquisition.c:8:2`, `<stdio.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-020` | `components/acquisition/src/acquisition.c:9:2`, `<time.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-021` | `components/ads1263/src/ads1263.c:3:2`, `<stdint.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-022` | `components/ads1263/src/ads1263.c:4:2`, `<string.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-023` | `components/ads1263/src/ads1263.c:5:2`, `<time.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-024` | `components/ads1263/arch/linux/src/ads1263_spi_linux.c:3:2`, `<errno.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-025` | `components/ads1263/arch/linux/src/ads1263_spi_linux.c:4:2`, `<fcntl.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-026` | `components/ads1263/arch/linux/src/ads1263_spi_linux.c:5:2`, `<linux/spi/spidev.h>` | Information; Cppcheck model limitation; header is accepted by GCC 15 | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-027` | `components/ads1263/arch/linux/src/ads1263_spi_linux.c:6:2`, `<stddef.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-028` | `components/ads1263/arch/linux/src/ads1263_spi_linux.c:7:2`, `<stdint.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-029` | `components/ads1263/arch/linux/src/ads1263_spi_linux.c:8:2`, `<string.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-030` | `components/ads1263/arch/linux/src/ads1263_spi_linux.c:9:2`, `<sys/ioctl.h>` | Information; Cppcheck model limitation affects side-effect knowledge | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-001`, `AR-SA-004` |
| `SA-ENV-031` | `components/ads1263/arch/linux/src/ads1263_spi_linux.c:10:2`, `<unistd.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-032` | `components/ads1263/arch/linux/src/ads1263_gpio_linux.c:3:2`, `<string.h>` | Information; Cppcheck system-header model limitation | `REPRODUCED` | Retained, not suppressed | No defect; `AR-SA-004` |
| `SA-ENV-033` | `main/main.c:0:0`, `normalCheckLevelMaxBranches` | Information; initial run did not exhaustively analyze every branch | Reproduced in `CP-R1`; absent by design in `CP-R2` | Resolved by exhaustive follow-up; original retained and not called clean | No defect; closed `AR-SA-005` |
| `SA-ENV-034` | `components/ads1263/src/ads1263.c:0:0`, `normalCheckLevelMaxBranches` | Information; initial run did not exhaustively analyze every branch | Reproduced in `CP-R1`; absent by design in `CP-R2` | Resolved by exhaustive follow-up; original retained and not called clean | No defect; closed `AR-SA-005` |
| `SA-ENV-035` | `ADS1263_NO_GPIOD=0`, configuration-wide | Blocker; real libgpiod-v2 implementation not analyzed | `GATE`: pkg-config exits 127; header probe exits 1 | `BLOCKED`; no clean or zero-finding claim; install prohibited | No defect; open `BLK-SA-001` |

## GCC 15 findings

No `SA-GCC15-*` record was created because `GCC-R1` produced no stdout or stderr diagnostic and exited 0. This is an informational compiler result for `ADS1263_NO_GPIOD=1`, not a Cppcheck finding, not an acceptance gate, and not evidence about the blocked libgpiod-v2 branch.

## Non-finding tool metadata

Cppcheck emitted `checkersReport` (`Active checkers: 119/975`) once per Cppcheck run. It is retained in raw stderr but is not a source finding, environment failure, or defect. Progress output proving `12/12 files checked 100% done` is retained in raw stdout and likewise is not counted as a finding.
