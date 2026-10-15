# Revision independiente de trazabilidad - Milestone 1

## Alcance

- Actividad: inspeccion documental; no es ejecucion de tests dinamicos.
- Artefactos: `docs/tsse/matriz_trazabilidad.md`, `docs/tsse/catalogo_casos.md`, `docs/tsse/cft_cases.md` y `docs/tsse/discrepancias.md`.
- Criterio: campos atomicos completos, trazabilidad bidireccional, estados normalizados y limites de afirmacion host/mock.

## Hallazgos corregidos

| ID | Hallazgo | Resolucion | Estado |
|---|---|---|---|
| TRA-R01 | Rangos REQ/ROB desactualizados y reverse mapping TST ausente | Se reconciliaron todos los IDs de matriz y catalogo | CORREGIDO |
| TRA-R02 | Algunos casos agrupaban varios vectores ejecutables bajo un estado comun | Cada vector ejecutable recibio ID, estado, evidencia, defecto, input y oracle propios | CORREGIDO |
| TRA-R03 | Destinos de evidencia abreviados no eran rutas literales | Cada caso usa un destino repository-relative completo | CORREGIDO |
| TRA-R04 | Una celda y una clasificacion no eran canonicas | Estados usan solo cinco valores y REQ-003 es PARTIAL HOST/MOCKED | CORREGIDO |
| TRA-R05 | Disposiciones DSP podian confundirse con estados de ejecucion | Se identificaron explicitamente como disposiciones del registro | CORREGIDO |
| TRA-R06 | Integracion scripted dependia de un gate no aprobado | INT-ACQ-004 queda `BLOCKED` con condicion de desbloqueo | CORREGIDO |
| TRA-R07 | Faltaban limites y transfer failure ordinario de WREG | Se agregaron casos atomicos count 0/32/33, start 32 y SPI_TRANSFER | CORREGIDO |
| TRA-R08 | ROB-003 no cubria las cuatro combinaciones DRDY/SPI ni consecuencia acquisition | Se agregaron GPIO/timeout con SPI success/failure, muestras concretas y cuatro consecuencias separadas | CORREGIDO |

## Disposicion

`APROBADO` despues de revision focalizada y auditoria integral final. Los registros contienen ID, fuente, nivel, tecnica, precondiciones, inputs/script, oracle, estado, evidencia y defecto. Las referencias son bidireccionales y los estados se limitan a `PASS`, `FAIL`, `BLOCKED`, `NOT RUN` y `OUT OF SCOPE`. Los casos no ejecutados no se presentan como resultados y las afirmaciones mock no cierran comportamiento fisico.
