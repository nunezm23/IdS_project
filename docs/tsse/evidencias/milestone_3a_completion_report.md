# Completion report - Milestone 3A

Run set: `milestone_3a_20261015T150616Z`.

## Resultado consolidado

| Medida | Ejecutados | PASS | FAIL | IGNORED | BLOCKED |
|---|---:|---:|---:|---:|---:|
| Inventario unitario final unico | 72 | 72 | 0 | 0 | 0 |
| Dos regresiones finales limpias por proyecto | 144 | 144 | 0 | 0 | 0 |
| Todas las ejecuciones Ceedling formales preservadas en Milestone 3A | 358 | 358 | 0 | 0 | 0 |
| Probe de proceso PROC-DSP017-001 | 1 | 0 | 1 | 0 | 0 |
| Total dinamico preservado, incluyendo repeticiones y probe | 359 | 358 | 1 | 0 | 0 |

El unico `FAIL` es el resultado de conformidad esperado de PROC-DSP017-001: una entrada CLI aceptada no termina antes del timeout externo y produce exit 124. No es un fallo de infraestructura. Los casos adapter y CFT que permanecen `NOT RUN` no se cuentan como ejecutados ni como `BLOCKED`.

Evidencia unitaria exacta:

- Baseline independiente: `docs/tsse/evidencias/04_unitarias/milestone_3a_20261015T150616Z/baseline/02_test_all.typescript`.
- ADS1263 final run 1: `docs/tsse/evidencias/04_unitarias/milestone_3a_20261015T150616Z/final/02_test_all_run_1.typescript`.
- ADS1263 final run 2: `docs/tsse/evidencias/04_unitarias/milestone_3a_20261015T150616Z/final/04_test_all_run_2.typescript`.
- Acquisition final run 1: `docs/tsse/evidencias/04_unitarias/milestone_3a_20261015T150616Z/final/08_acquisition_test_run_1.typescript`.
- Acquisition final run 2: `docs/tsse/evidencias/04_unitarias/milestone_3a_20261015T150616Z/final/10_acquisition_test_run_2.typescript`.
- Inventario y resultados: `docs/tsse/evidencias/04_unitarias/milestone_3a_20261015T150616Z/README.md`.

## Sanitizers

ROB-001 ejecuto dos casos por perfil. Normal, ASan y UBSan terminan 0 con 2/2 PASS. ASan y UBSan no emiten diagnosticos. El resultado no demuestra seguridad: el acceso cruza el subobjeto `mock_registers` pero permanece dentro del objeto contenedor `ads1263_t`.

- Normal: `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/05_rob001_normal.typescript`.
- ASan: `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/07_rob001_asan.typescript`.
- UBSan: `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/09_rob001_ubsan.typescript`.

## Cobertura

| Modulo | Baseline | Milestone 3A final |
|---|---|---|
| `ads1263.c` lineas | 151/222 (68.0%) | 151/222 (68.0%) |
| `ads1263.c` funciones | 13/16 (81.2%) | 13/16 (81.2%) |
| `ads1263.c` ramas | 90/134 (67.2%) | 90/134 (67.2%) |
| `acquisition.c` lineas | No incluido en el baseline reproducido | 54/168 (32.1%) |
| `acquisition.c` funciones | No incluido en el baseline reproducido | 6/13 (46.2%) |
| `acquisition.c` ramas | No incluido en el baseline reproducido | 26/76 (34.2%) |

- Baseline: `docs/tsse/evidencias/06_cobertura/milestone_3a_20261015T150616Z/baseline/`.
- Final: `docs/tsse/evidencias/06_cobertura/milestone_3a_20261015T150616Z/final/`.
- Denominadores y exclusiones: `docs/tsse/evidencias/06_cobertura/milestone_3a_20261015T150616Z/README.md`.

## Defectos y anomalias

| ID | Resultado | Disposicion | Evidencia |
|---|---|---|---|
| ROB-001 / FND-ROB001-001 | Lectura y escritura del byte posterior al arreglo para start=31,count=2 | `SUSPECTED DEFECT`; contrato de rechazo pendiente | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` |
| ROB-002 | Fallos SPI core propagados y efectos atomicos reproducidos | `EXPECTED BEHAVIOR` core; adapters/CFT `NOT RUN` | Mismo reporte y runs unitarios |
| ROB-003 | Estados DRDY diagnosticos y consecuencias acquisition reproducidos | `CONTRACT AMBIGUITY` | Mismo reporte y runs acquisition finales |
| DEF-001 / DSP-017 | rate=1 genera `{0,1000000000}`, POSIX retorna EINVAL y N=2 no termina | `CONFIRMED DEFECT`; correccion pendiente de gate humano | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md` |

Anomalias de infraestructura preservadas y corregidas solo en soporte de test:

- Enlace inicial del probe sin fake DRDY: `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/01_dsp017_probe_build.typescript`; rerun exitoso `02_dsp017_probe_build_pass.typescript`.
- Carrera de export gcovr: `docs/tsse/evidencias/06_cobertura/milestone_3a_20261015T150616Z/final/04_ads1263_gcovr_artifacts.typescript`; rerun serial exitoso `05_ads1263_gcovr_artifacts_pass.typescript`.
- Intentos de desarrollo acquisition anteriores al run formal no tuvieron transcript raw; la limitacion esta declarada en el reporte de investigaciones y no se presentan como evidencia reproducible.

## Gate

Milestone 3A no modifica C/H de produccion ni el `Makefile` de produccion. DEF-001 requiere decision humana y una actualizacion OpenSpec aprobada o un cambio separado antes de editar produccion. ROB-001 y ROB-003 conservan las decisiones contractuales indicadas en sus disposiciones.
