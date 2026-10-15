# Unit tests y CFT - Milestone 4

| Suite | Resultado formal | Evidencia |
|---|---:|---|
| ADS1263 | 66/66 PASS, 0 FAIL, 0 IGNORED | `03_ads1263_clean_test_all_pass.typescript`; `ads1263_tests_report.json`; JUnit |
| Acquisition | 30/30 PASS, 0 FAIL, 0 IGNORED | `04_acquisition_cft_clean_test_all_after_fix.typescript`; `acquisition_tests_report.json`; JUnit |

Acquisition contiene los seis casos de robustez existentes y exactamente `CFT-P01..P24`; los 24 CFT pasaron. `01_ads1263_clean_test_all.typescript` preserva el intento inicial que no enlazo por alcance accidental de `stdio_wrapper` (`__real_fopen/fputs/fclose`); es un fallo de infraestructura, no un test FAIL. El rerun ADS1263 limpio paso. `02_*` y `04_*` preservan ejecuciones acquisition 30/30.

El inventario unitario no CFT es `72`: 66 ADS1263 mas 6 acquisition. Los 24 CFT son un inventario separado, aunque se ejecutan dentro de la suite acquisition de 30 tests; no se suman como parte de esos 72.

La identidad de inputs no es HEAD solamente: las ejecuciones usaron HEAD `54a91ed4b684b5ac6f4221c8743768fad5083270` mas cambios test/infraestructura dirty. Sus hashes exactos estan en `../../01_proyecto/milestone_4_20261015T161203Z/test_input_checksums.typescript`, capturado despues de la ejecucion a partir de los inputs sin cambios. No existe una revision test pre-ejecucion versionada.

Se usa produccion real con dependencias controladas; no hay afirmacion de hardware.
