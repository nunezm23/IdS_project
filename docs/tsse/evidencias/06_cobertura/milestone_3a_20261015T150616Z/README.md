# Cobertura - Milestone 3A

## Baseline reproducido

El run pre-cambio de 62/62 tests reprodujo para `components/ads1263/src/ads1263.c`: 151/222 lineas (68.0%), 13/16 funciones (81.2%) y 90/134 branches (67.2%). Evidencia en `baseline/`.

## Medicion final

| Modulo | Suite | Lineas | Funciones | Branches |
|---|---|---:|---:|---:|
| `components/ads1263/src/ads1263.c` | 66/66 PASS | 151/222 (68.0%) | 13/16 (81.2%) | 90/134 (67.2%) |
| `components/acquisition/src/acquisition.c` | 6/6 PASS | 54/168 (32.1%) | 6/13 (46.2%) | 26/76 (34.2%) |

La cobertura ADS1263 no cambia frente al baseline: los casos nuevos ejercen combinaciones y efectos ya contenidos en lineas/branches previamente cubiertos. Acquisition es una nueva medicion de modulo para la suite de robustez y no se suma al denominador historico ADS1263. No se aplica umbral de aceptacion.

Los artefactos finales autoritativos son `final/ads1263_coverage_summary.json`, `final/ads1263_coverage.json`, `final/ads1263_coverage.html`, `final/acquisition_coverage_summary_final.json`, `final/acquisition_coverage_final.json` y `final/acquisition_coverage_final.html`, junto con sus transcripts y reportes JSON/JUnit.

`final/04_ads1263_gcovr_artifacts.typescript` es un fallo de infraestructura por ejecutar dos gcovr concurrentes; `final/05_ads1263_gcovr_artifacts_pass.typescript` conserva el rerun serial exitoso. Las metricas citadas provienen solo de exports exitosos.
