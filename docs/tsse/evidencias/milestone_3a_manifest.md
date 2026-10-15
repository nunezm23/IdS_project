# Manifest de evidencia - Milestone 3A

Run set: `milestone_3a_20261015T150616Z`.

| Actividad/casos | Estado | Evidencia principal | Raw/machine-readable |
|---|---|---|---|
| Entorno, procedencia y configuraciones | `PASS` | `01_proyecto/milestone_3a_20261015T150616Z/README.md` | Transcripts y `processed_*.yml` |
| Baseline independiente 62 tests | `PASS` | `04_unitarias/milestone_3a_20261015T150616Z/README.md` | `baseline/02_test_all.typescript`; JSON/JUnit |
| Dos runs ADS1263 finales, 66 tests | `PASS` | `04_unitarias/milestone_3a_20261015T150616Z/README.md` | `final/02_test_all_run_1.typescript`; `final/04_test_all_run_2.typescript`; JSON/JUnit |
| Dos runs acquisition finales, 6 tests | `PASS` | `04_unitarias/milestone_3a_20261015T150616Z/README.md` | `final/08_acquisition_test_run_1.typescript`; `final/10_acquisition_test_run_2.typescript`; JSON/JUnit |
| ROB-001 normal/ASan/UBSan | `PASS` caracterizacion | `07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | `05_rob001_normal.typescript`; `07_rob001_asan.typescript`; `09_rob001_ubsan.typescript` |
| ROB-002 core | `PASS` parcial | mismo reporte | Runs unitarios; adapters/CFT `NOT RUN` |
| ROB-003 core y consecuencia acquisition | `PASS` caracterizacion | mismo reporte | Runs unitarios finales |
| PROC-DSP017-001 | `FAIL` esperado de conformance; DEF-001 | `07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md` | `03_dsp017_probe_timeout.typescript`, exit 124 |
| Coverage ADS1263/acquisition | `PASS` | `06_cobertura/milestone_3a_20261015T150616Z/README.md` | JSON, HTML, txt, JSON/JUnit y transcripts |
| Completion report | `PASS` | `milestone_3a_completion_report.md` | Totales, coverage, hallazgos y rutas exactas |
| Checksums finales | `PASS` | este manifest | `milestone_3a_checksums.typescript` |

Los manifests de Milestone 2A/2B y sus artefactos permanecen sin reemplazo. No se ejecutaron hardware, adapters, CFT general, integracion general ni correcciones de produccion.
