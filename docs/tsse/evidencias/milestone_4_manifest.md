# Manifest de evidencia - Milestone 4

Run set: `milestone_4_20261015T161203Z`.

Revision Git: `54a91ed4b684b5ac6f4221c8743768fad5083270`. Las ejecuciones formales usaron esa revision mas inputs test/infraestructura dirty identificados por `01_proyecto/milestone_4_20261015T161203Z/test_input_checksums.typescript`; HEAD solo no reproduce el run. El manifest se capturo despues de la ejecucion sobre inputs que permanecieron sin cambios, y no existe un commit o manifest pre-ejecucion de esos cambios. Baseline de produccion: `c59b24017f5daebb4e37512d799a0d07f589bce5`; diff de C/H/Makefile vacio.

| Actividad/casos | Estado | Evidencia principal | Raw/machine-readable |
|---|---|---|---|
| Entorno, procedencia y guarda | `PASS` | `01_proyecto/milestone_4_20261015T161203Z/README.md` | Transcripts de metadata y production guard |
| Inputs exactos test/infraestructura | Manifest post-ejecucion | mismo README | `01_proyecto/milestone_4_20261015T161203Z/test_input_checksums.typescript` |
| ADS1263 actual | `PASS`, 66/66 | `04_unitarias/milestone_4_20261015T161203Z/README.md` | `03_ads1263_clean_test_all_pass.typescript`; JSON/JUnit |
| Intento ADS1263 inicial | Infraestructura `FAIL`, preservado | mismo README | `01_ads1263_clean_test_all.typescript`; link de stdio wrapper |
| Acquisition actual | `PASS`, 30/30 | mismo README | `04_acquisition_cft_clean_test_all_after_fix.typescript`; JSON/JUnit |
| CFT-P01..P24 | `PASS`, 24/24 | `cft_cases.md`; catalogo | `acquisition_tests_report.json` con 24 nombres exactos |
| Integracion host run 1 | `PASS`, 71/71, 0 fail/blocked | `05_integracion_host/milestone_4_20261015T161203Z/README.md` | `run_1/summary.json`; evidencia por caso |
| Integracion host run 2 | `PASS`, 71/71, 0 fail/blocked | mismo README | `run_2/summary.json`; evidencia por caso |
| Registro canonico integracion | `PASS` auditoria | `08_trazabilidad/milestone_4_20261015T161203Z/integration_case_traceability.md` | 71 IDs, fuente y dos rutas por caso |
| Catalogo legacy de integracion | 19 `PASS`, 10 `NOT RUN` | `catalogo_casos.md`; registro canonico | CLI-006-F conserva su input exacto; `CLI-LIVE-CSV` es cercano, no identico |
| Cppcheck no-gpiod | Completado con findings | `02_analisis_estatico/milestone_4_20261015T161203Z/README.md` | 41 style, 32 missing includes, metadata/limites; `findings.md` |
| Cppcheck/GCC libgpiod-v2 | `BLOCKED` | mismo README | falta `pkg-config` y `gpiod.h`; comandos no ejecutados preservados |
| GCC 15 fallback no-gpiod | Completado, 0 diagnosticos | mismo README | `raw_analysis/003_gcc15_no_gpiod.*` |
| Coverage ADS1263 | `PASS` medicion | `06_cobertura/milestone_4_20261015T161203Z/README.md` | 151/222 lineas, 13/16 funciones, 90/134 branches |
| Coverage acquisition | `PASS` medicion serial | mismo README | 118/168 lineas, 9/13 funciones, 53/76 branches |
| Intento gcovr acquisition paralelo | Infraestructura `FAIL`, preservado | mismo README | `02_acquisition_coverage.typescript`; rerun `03_*` PASS |
| Versiones post-run de coverage | Captura post-run | mismo README | `04_tool_versions_post_run.typescript`; limitacion temporal declarada |
| Auditorias finales | `PASS` documental | `08_trazabilidad/milestone_4_20261015T161203Z/` | raw-to-summary, bidireccional, defectos, inventario y reconciliacion independiente; no implica aprobacion humana |
| DEF-001 | Abierto, sin cambio | auditoria de defectos y evidencia Milestone 3A | No se modifico produccion; ROB conservados |
| Deck y material de presentacion | `PASS` tecnico automatizado | `09_presentacion/milestone_4_20261015T161203Z/README.md` | PPTX, generador, checksum actual, paquete/XML y determinismo; snapshot post-generacion |
| Aprobacion humana reporte/rehearsal | `BLOCKED / PENDING HUMAN` | inventario final; estado de rehearsal | No se declara cierre humano |

Cppcheck no confirma defectos nuevos: 38 diagnostics se disponen como falsos positivos de contexto/herramienta y tres como observaciones aceptadas de interfaz publica. DSP-005/006/007/008 quedan caracterizados por integracion sin convertir ambiguedad en defecto.

Los manifests Milestone 2A, 2B y 3A permanecen historicos y no son reemplazados. Toda evidencia dinamica Milestone 4 es host/software-only; no verifica hardware, target, SPI/GPIO fisicos, ADC ni tiempo real.

Excepcion append-only de Milestone 4: durante correcciones de esta misma campaña se modificaron `02_analisis_estatico/.../00_policy.md`, `README.md` y `05_finalize_evidence.sh`; el addendum `02_analisis_estatico/.../07_integrity_addendum.md` preserva hashes/verificaciones anteriores y declara las diferencias. En presentacion, el PPTX previo fue reemplazado en el mismo destino y solo se conservaron sus hashes supersedidos. Esos artefactos anteriores no pueden reconstruirse ni declararse preservados; por ello la verificacion estricta de no overwrite permanece abierta.
