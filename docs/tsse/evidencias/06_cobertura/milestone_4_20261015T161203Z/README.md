# Cobertura - Milestone 4

| Modulo | Baseline anterior | Milestone 4 |
|---|---|---|
| `ads1263.c` | 151/222 lineas; 13/16 funciones; 90/134 branches | 151/222; 13/16; 90/134, sin cambio |
| `acquisition.c` | 54/168 lineas; 6/13 funciones; 26/76 branches | 118/168; 9/13; 53/76 |

Los artefactos autoritativos estan en `ads1263/` y `acquisition/` (`GcovCoverage.json`, HTML, `coverage.txt`, JSON/JUnit). No se aplica umbral inventado.

`02_acquisition_coverage.typescript` preserva un fallo gcovr por carrera contra el arbol coverage compartido durante ejecucion paralela; los 30 tests habian pasado, pero ese export no es autoritativo. `03_acquisition_coverage_serial_pass.typescript` preserva el rerun serial exitoso del que provienen los artefactos publicados.

Los perfiles se ejecutaron sobre HEAD `54a91ed4b684b5ac6f4221c8743768fad5083270` mas inputs test/infraestructura dirty. `../../01_proyecto/milestone_4_20261015T161203Z/test_input_checksums.typescript` identifica esos inputs exactos; fue capturado despues de las ejecuciones, cuando permanecian sin cambios, y no sustituye un commit o manifest pre-ejecucion inexistente. HEAD solo no basta para reproducir estas metricas.

`04_tool_versions_post_run.typescript` registra GCC/gcov `15.2.0`, gcovr `7.2`, Ceedling `1.1.10`, CMock/Unity `2.7.2`, fecha UTC, working directory y HEAD. Es una captura post-run del mismo entorno sin cambios, no metadata tomada simultaneamente con los exports; esta limitacion temporal se conserva expresamente.
