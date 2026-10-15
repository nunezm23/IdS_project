# Registro canonico de integracion ejecutada - Milestone 4

Este documento es el suplemento canonico de `docs/tsse/catalogo_casos.md` para los 71 IDs de integracion realmente ejecutados. La raiz abreviada `I` es `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/`. Cada ruta apunta al resultado por caso; en el mismo directorio estan comando, assertions, stdout/stderr, exit y artefactos adicionales.

Todos los casos estan `PASS` en los dos runs independientes. `run_1/summary.json` y `run_2/summary.json` informan 71 PASS, 0 FAIL, 0 BLOCKED y coincidencia exacta entre declaracion y ejecucion.

| ID ejecutado | Fuente REQ/TST/ROB | Estado | Evidencia exacta por caso |
|---|---|---|---|
| INT-MOCK-CHECK | REQ-002, TST-006 | `PASS` | `I/run_1/cases/INT-MOCK-CHECK/result.json`; `I/run_2/cases/INT-MOCK-CHECK/result.json` |
| INT-MOCK-SAMPLES | REQ-008, TST-006 | `PASS` | `I/run_1/cases/INT-MOCK-SAMPLES/result.json`; `I/run_2/cases/INT-MOCK-SAMPLES/result.json` |
| INT-CSV-CREATE | REQ-010, TST-006 | `PASS` | `I/run_1/cases/INT-CSV-CREATE/result.json`; `I/run_2/cases/INT-CSV-CREATE/result.json` |
| INT-CSV-TRUNCATE | REQ-010, TST-006 | `PASS` caracterizacion | `I/run_1/cases/INT-CSV-TRUNCATE/result.json`; `I/run_2/cases/INT-CSV-TRUNCATE/result.json` |
| INT-CSV-FAIL | REQ-010, TST-006 | `PASS` caracterizacion | `I/run_1/cases/INT-CSV-FAIL/result.json`; `I/run_2/cases/INT-CSV-FAIL/result.json` |
| CLI-NO-MODE | REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-NO-MODE/result.json`; `I/run_2/cases/CLI-NO-MODE/result.json` |
| CLI-HELP | REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-HELP/result.json`; `I/run_2/cases/CLI-HELP/result.json` |
| CLI-UNKNOWN | REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-UNKNOWN/result.json`; `I/run_2/cases/CLI-UNKNOWN/result.json` |
| CLI-MISSING-SAMPLES | REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MISSING-SAMPLES/result.json`; `I/run_2/cases/CLI-MISSING-SAMPLES/result.json` |
| CLI-MISSING-CHANNEL | REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MISSING-CHANNEL/result.json`; `I/run_2/cases/CLI-MISSING-CHANNEL/result.json` |
| CLI-MISSING-RATE | REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MISSING-RATE/result.json`; `I/run_2/cases/CLI-MISSING-RATE/result.json` |
| CLI-MISSING-DRDY | REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MISSING-DRDY/result.json`; `I/run_2/cases/CLI-MISSING-DRDY/result.json` |
| CLI-MISSING-CSV | REQ-010, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MISSING-CSV/result.json`; `I/run_2/cases/CLI-MISSING-CSV/result.json` |
| CLI-MISSING-SPI | REQ-002, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MISSING-SPI/result.json`; `I/run_2/cases/CLI-MISSING-SPI/result.json` |
| CLI-MISSING-GPIO | REQ-006, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MISSING-GPIO/result.json`; `I/run_2/cases/CLI-MISSING-GPIO/result.json` |
| CLI-MALFORMED-SAMPLES | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MALFORMED-SAMPLES/result.json`; `I/run_2/cases/CLI-MALFORMED-SAMPLES/result.json` |
| CLI-MALFORMED-RATE | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MALFORMED-RATE/result.json`; `I/run_2/cases/CLI-MALFORMED-RATE/result.json` |
| CLI-MALFORMED-CHANNEL | REQ-007, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MALFORMED-CHANNEL/result.json`; `I/run_2/cases/CLI-MALFORMED-CHANNEL/result.json` |
| CLI-MALFORMED-DRDY | REQ-006, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-MALFORMED-DRDY/result.json`; `I/run_2/cases/CLI-MALFORMED-DRDY/result.json` |
| CLI-EMPTY-SAMPLES | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-EMPTY-SAMPLES/result.json`; `I/run_2/cases/CLI-EMPTY-SAMPLES/result.json` |
| CLI-EMPTY-RATE | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-EMPTY-RATE/result.json`; `I/run_2/cases/CLI-EMPTY-RATE/result.json` |
| CLI-EMPTY-CHANNEL | REQ-007, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-EMPTY-CHANNEL/result.json`; `I/run_2/cases/CLI-EMPTY-CHANNEL/result.json` |
| CLI-EMPTY-DRDY | REQ-006, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-EMPTY-DRDY/result.json`; `I/run_2/cases/CLI-EMPTY-DRDY/result.json` |
| CLI-OVERFLOW-SAMPLES | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-OVERFLOW-SAMPLES/result.json`; `I/run_2/cases/CLI-OVERFLOW-SAMPLES/result.json` |
| CLI-OVERFLOW-RATE | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-OVERFLOW-RATE/result.json`; `I/run_2/cases/CLI-OVERFLOW-RATE/result.json` |
| CLI-OVERFLOW-CHANNEL | REQ-007, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-OVERFLOW-CHANNEL/result.json`; `I/run_2/cases/CLI-OVERFLOW-CHANNEL/result.json` |
| CLI-OVERFLOW-DRDY | REQ-006, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-OVERFLOW-DRDY/result.json`; `I/run_2/cases/CLI-OVERFLOW-DRDY/result.json` |
| CLI-SAMPLES-0 | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-SAMPLES-0/result.json`; `I/run_2/cases/CLI-SAMPLES-0/result.json` |
| CLI-SAMPLES-1 | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-SAMPLES-1/result.json`; `I/run_2/cases/CLI-SAMPLES-1/result.json` |
| CLI-SAMPLES-1000 | REQ-008, REQ-011, TST-006 | `PASS` parser | `I/run_1/cases/CLI-SAMPLES-1000/result.json`; `I/run_2/cases/CLI-SAMPLES-1000/result.json` |
| CLI-SAMPLES-1001 | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-SAMPLES-1001/result.json`; `I/run_2/cases/CLI-SAMPLES-1001/result.json` |
| CLI-RATE-0 | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-RATE-0/result.json`; `I/run_2/cases/CLI-RATE-0/result.json` |
| CLI-RATE-1 | REQ-008, REQ-011, TST-006 | `PASS` N=1 | `I/run_1/cases/CLI-RATE-1/result.json`; `I/run_2/cases/CLI-RATE-1/result.json` |
| CLI-RATE-10 | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-RATE-10/result.json`; `I/run_2/cases/CLI-RATE-10/result.json` |
| CLI-RATE-11 | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-RATE-11/result.json`; `I/run_2/cases/CLI-RATE-11/result.json` |
| CLI-CHANNEL-0 | REQ-007, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-CHANNEL-0/result.json`; `I/run_2/cases/CLI-CHANNEL-0/result.json` |
| CLI-CHANNEL-1 | REQ-007, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-CHANNEL-1/result.json`; `I/run_2/cases/CLI-CHANNEL-1/result.json` |
| CLI-DRDY-0 | REQ-006, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-DRDY-0/result.json`; `I/run_2/cases/CLI-DRDY-0/result.json` |
| CLI-DRDY-1023 | REQ-006, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-DRDY-1023/result.json`; `I/run_2/cases/CLI-DRDY-1023/result.json` |
| CLI-DRDY-1024 | REQ-006, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-DRDY-1024/result.json`; `I/run_2/cases/CLI-DRDY-1024/result.json` |
| CLI-CHECK-SAMPLES | REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-CHECK-SAMPLES/result.json`; `I/run_2/cases/CLI-CHECK-SAMPLES/result.json` |
| CLI-SAMPLES-CHECK | REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-SAMPLES-CHECK/result.json`; `I/run_2/cases/CLI-SAMPLES-CHECK/result.json` |
| CLI-CHECK-LIVE | REQ-009, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-CHECK-LIVE/result.json`; `I/run_2/cases/CLI-CHECK-LIVE/result.json` |
| CLI-LIVE-CHECK | REQ-009, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-LIVE-CHECK/result.json`; `I/run_2/cases/CLI-LIVE-CHECK/result.json` |
| CLI-HELP-CHECK | REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-HELP-CHECK/result.json`; `I/run_2/cases/CLI-HELP-CHECK/result.json` |
| CLI-CHECK-HELP | REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-CHECK-HELP/result.json`; `I/run_2/cases/CLI-CHECK-HELP/result.json` |
| CLI-REPEATED-CHECK | REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-REPEATED-CHECK/result.json`; `I/run_2/cases/CLI-REPEATED-CHECK/result.json` |
| CLI-SAMPLES-LIVE | REQ-009, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-SAMPLES-LIVE/result.json`; `I/run_2/cases/CLI-SAMPLES-LIVE/result.json` |
| CLI-LIVE-SAMPLES | REQ-009, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-LIVE-SAMPLES/result.json`; `I/run_2/cases/CLI-LIVE-SAMPLES/result.json` |
| CLI-SAMPLES-HELP | REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-SAMPLES-HELP/result.json`; `I/run_2/cases/CLI-SAMPLES-HELP/result.json` |
| CLI-HELP-SAMPLES | REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-HELP-SAMPLES/result.json`; `I/run_2/cases/CLI-HELP-SAMPLES/result.json` |
| CLI-LIVE-HELP | REQ-009, REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-LIVE-HELP/result.json`; `I/run_2/cases/CLI-LIVE-HELP/result.json` |
| CLI-HELP-LIVE | REQ-009, REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-HELP-LIVE/result.json`; `I/run_2/cases/CLI-HELP-LIVE/result.json` |
| CLI-REPEATED-SAMPLES | REQ-008, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-REPEATED-SAMPLES/result.json`; `I/run_2/cases/CLI-REPEATED-SAMPLES/result.json` |
| CLI-REPEATED-LIVE | REQ-009, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-REPEATED-LIVE/result.json`; `I/run_2/cases/CLI-REPEATED-LIVE/result.json` |
| CLI-REPEATED-HELP | REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-REPEATED-HELP/result.json`; `I/run_2/cases/CLI-REPEATED-HELP/result.json` |
| CLI-LIVE-CSV | REQ-009, REQ-010, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-LIVE-CSV/result.json`; `I/run_2/cases/CLI-LIVE-CSV/result.json` |
| CLI-CSV-LIVE | REQ-009, REQ-010, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-CSV-LIVE/result.json`; `I/run_2/cases/CLI-CSV-LIVE/result.json` |
| CLI-CHECK-CSV | REQ-010, REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-CHECK-CSV/result.json`; `I/run_2/cases/CLI-CHECK-CSV/result.json` |
| CLI-CSV-CHECK | REQ-010, REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-CSV-CHECK/result.json`; `I/run_2/cases/CLI-CSV-CHECK/result.json` |
| CLI-HELP-CSV | REQ-010, REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-HELP-CSV/result.json`; `I/run_2/cases/CLI-HELP-CSV/result.json` |
| CLI-CSV-HELP | REQ-010, REQ-011, TST-006 | `PASS` caracterizacion | `I/run_1/cases/CLI-CSV-HELP/result.json`; `I/run_2/cases/CLI-CSV-HELP/result.json` |
| CLI-CSV-NO-MODE | REQ-010, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-CSV-NO-MODE/result.json`; `I/run_2/cases/CLI-CSV-NO-MODE/result.json` |
| CLI-CSV-SAMPLES | REQ-008, REQ-010, REQ-011, TST-006 | `PASS` | `I/run_1/cases/CLI-CSV-SAMPLES/result.json`; `I/run_2/cases/CLI-CSV-SAMPLES/result.json` |
| INT-STRACE-NO-DEVICE | REQ-002, TST-006 | `PASS` | `I/run_1/cases/INT-STRACE-NO-DEVICE/result.json`; `I/run_2/cases/INT-STRACE-NO-DEVICE/result.json` |
| INT-CSV-CLOSE-STRACE | REQ-010, TST-006 | `PASS` | `I/run_1/cases/INT-CSV-CLOSE-STRACE/result.json`; `I/run_2/cases/INT-CSV-CLOSE-STRACE/result.json` |
| INT-LIVE-SIGINT | REQ-009, TST-006 | `PASS` | `I/run_1/cases/INT-LIVE-SIGINT/result.json`; `I/run_2/cases/INT-LIVE-SIGINT/result.json` |
| HAL-HAPPY-SAMPLE | REQ-004, REQ-007, REQ-008, TST-006 | `PASS` | `I/run_1/cases/HAL-HAPPY-SAMPLE/result.json`; `I/run_2/cases/HAL-HAPPY-SAMPLE/result.json` |
| HAL-DELAYED-DRDY | REQ-006, REQ-008, ROB-003, TST-006 | `PASS` | `I/run_1/cases/HAL-DELAYED-DRDY/result.json`; `I/run_2/cases/HAL-DELAYED-DRDY/result.json` |
| HAL-READ-FAILURE | REQ-008, ROB-002, TST-006 | `PASS` | `I/run_1/cases/HAL-READ-FAILURE/result.json`; `I/run_2/cases/HAL-READ-FAILURE/result.json` |
| HAL-PRESENCE-FAILURE | REQ-005, TST-006 | `PASS` | `I/run_1/cases/HAL-PRESENCE-FAILURE/result.json`; `I/run_2/cases/HAL-PRESENCE-FAILURE/result.json` |

## IDs legacy no cerrados por equivalencia

Permanecen `NOT RUN`: `INT-ACQ-001`, `INT-ACQ-002`, `INT-CSV-001`, `INT-CSV-002`, `INT-CSV-003`, `INT-LIVE-001`, `INT-LIVE-002`, `CLI-004-D`, `CLI-005` y `CLI-006-F`. El catalogo enlaza escenarios cercanos, pero difieren en rate, cantidad de muestras, argumento malformado, opciones adicionales, ruta CSV o momento del SIGINT. En particular, CLI-006-F exige exactamente `--live --channel 0 --rate-hz 2 --csv out.csv --mock`, mientras `CLI-LIVE-CSV` ejecuto `--live --csv unused.csv --mock`; aunque observo el mismo diagnostico, no es el mismo input. No se transfirio `PASS` entre inputs/oraculos no identicos.

## Limite

Los 71 IDs verifican procesos host con mock, fake HAL, `strace` o PTY segun `I/run_1/scenarios.md` y `I/run_2/scenarios.md`. No verifican hardware, libgpiod real, buses fisicos, electricidad, exactitud ADC, target ni tiempo real.
