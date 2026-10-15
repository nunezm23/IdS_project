# Integracion host - Milestone 4

Dos ejecuciones independientes preservadas tienen inventario y estados identicos: cada una declara y ejecuta 71 IDs, con `71 PASS`, `0 FAIL`, `0 BLOCKED`, sin IDs omitidos ni extra.

| Run | Resumen | Evidencia por caso |
|---|---|---|
| 1 | `run_1/summary.json` | `run_1/cases/<ID>/result.json`, assertions, comando, stdout/stderr y exit |
| 2 | `run_2/summary.json` | `run_2/cases/<ID>/result.json`, assertions, comando, stdout/stderr y exit |

`build_manifest.json` fija fuentes, hashes, compilador y limites. `run_1/scenarios.md` y `run_2/scenarios.md` conservan los 71 oraculos. El registro REQ/TST/ROB canonico esta en `../../08_trazabilidad/milestone_4_20261015T161203Z/integration_case_traceability.md`.

Los perfiles mock/fake-HAL/strace/PTY verifican software host. No verifican Linux libgpiod real, SPI/GPIO fisicos, target, electricidad, exactitud ADC ni rendimiento real-time.
