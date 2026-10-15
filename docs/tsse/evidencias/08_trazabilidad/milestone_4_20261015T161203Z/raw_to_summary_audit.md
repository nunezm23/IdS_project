# Auditoria raw-to-summary - Milestone 4

Run: `milestone_4_20261015T161203Z`; HEAD `54a91ed4b684b5ac6f4221c8743768fad5083270` mas inputs test/infraestructura dirty identificados por `01_proyecto/.../test_input_checksums.typescript`. HEAD solo no reproduce el run; el manifest fue capturado despues de la ejecucion sobre inputs sin cambios y no existe snapshot versionado pre-ejecucion.

| Afirmacion resumida | Raw/machine-readable | Disposicion |
|---|---|---|
| Produccion sin diff contra `c59b240...` | `01_proyecto/.../production_guard_before.typescript`; static `raw_metadata/005_*`, `006_*` y `raw_analysis/006_*` | Confirmada, exit 0/vacio |
| Identidad exacta de inputs test/infraestructura | `01_proyecto/.../test_input_checksums.typescript`; `git_status.typescript` | Identificada post-ejecucion; no se presenta como commit ni captura pre-ejecucion |
| ADS1263 66/66 PASS | `04_unitarias/.../03_ads1263_clean_test_all_pass.typescript`; `ads1263_tests_report.json`; JUnit | Confirmada |
| Fallo inicial ADS1263 de infraestructura | `04_unitarias/.../01_ads1263_clean_test_all.typescript` | Preservado; link de stdio wrapper, no test FAIL |
| Acquisition 30/30, incluidos CFT 24/24 | `04_unitarias/.../04_acquisition_cft_clean_test_all_after_fix.typescript`; `acquisition_tests_report.json`; JUnit | Confirmada por nombres exactos |
| Integracion 71/71 dos veces | `05_integracion_host/.../run_1/summary.json`; `run_2/summary.json`; cada `cases/<ID>/result.json` | Confirmada; 0 fail/blocked y mismo inventario |
| Cppcheck 41 style, 32 missing includes, cero defectos | `02_analisis_estatico/.../raw_analysis_exhaustive/`; `findings.md` | Confirmada; 38 falsos positivos y tres observaciones aceptadas |
| libgpiod-v2 bloqueado | static `raw_metadata/015_*`, `017_*`, `019_*`; `raw_analysis/002_*`, `004_*` | Confirmada; faltan pkg-config/header |
| GCC fallback cero diagnosticos | static `raw_analysis/003_gcc15_no_gpiod.*` | Confirmada; resultado informativo |
| Cobertura ADS1263 151/222, 13/16, 90/134 | `06_cobertura/.../ads1263/GcovCoverage.json`, HTML y txt | Confirmada, sin cambio |
| Cobertura acquisition 118/168, 9/13, 53/76 | `06_cobertura/.../acquisition/GcovCoverage.json`, HTML y txt | Confirmada, mejora sobre baseline |
| Carrera gcovr y rerun serial | `06_cobertura/.../02_acquisition_coverage.typescript`; `03_acquisition_coverage_serial_pass.typescript` | Fallo preservado; solo export serial es autoritativo |

Las elipsis de esta tabla significan la raiz `docs/tsse/evidencias/<area>/milestone_4_20261015T161203Z/`; no identifican evidencia fuera del run.
