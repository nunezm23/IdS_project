# Reconciliacion de revision independiente - Milestone 4

Esta nota registra correcciones documentales; no modifica evidencia raw, tests, produccion ni estados de aprobacion humana.

| Hallazgo | Reconciliacion | Resultado documental |
|---|---|---|
| CLI-006-F habia sido equiparado con `CLI-LIVE-CSV` | Se recupero de `HEAD:docs/tsse/catalogo_casos.md` el input `--live --channel 0 --rate-hz 2 --csv out.csv --mock` y su oracle exacto. El escenario ejecutado uso `--live --csv unused.csv --mock` | CLI-006-F vuelve a `NOT RUN`; `CLI-LIVE-CSV` queda como evidencia cercana, no identica |
| Inventario legacy informado como 20 PASS/9 NOT RUN | Se reconciliaron catalogo, matriz, suplemento, auditoria bidireccional e inventario final | Estado corregido: `19 PASS`, `10 NOT RUN`; CLI-006-F se enumera entre los pendientes |
| HEAD se podia interpretar como identidad completa del run | Se enlazo `01_proyecto/milestone_4_20261015T161203Z/test_input_checksums.typescript` | La identidad se expresa como HEAD mas inputs test/infraestructura dirty; el manifest es post-ejecucion y no se inventa una revision pre-ejecucion |
| Inventarios finales estaban dispersos | Se hizo explicito el inventario `NOT RUN`, `BLOCKED`, defectos/decisiones abiertas y el limite host-only | No se declara cierre tecnico total ni aprobacion humana |

Los dos runs formales de integracion conservan los mismos 71 IDs y los mismos estados: 71 `PASS`, 0 `FAIL`, 0 `BLOCKED` en cada uno. Esta repeticion no convierte escenarios cercanos en ejecuciones exactas de IDs legacy.

Estado normalizado de revision humana del informe y del rehearsal: `BLOCKED / PENDING HUMAN`; se desbloquean con aprobacion factual documentada y rehearsal humano registrado, respectivamente.
