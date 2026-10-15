# Matriz de trazabilidad - TSSE

## Referencia

- Proyecto: `IdS_project`.
- Baseline de produccion: `c59b24017f5daebb4e37512d799a0d07f589bce5`.
- Revision documental al iniciar Milestone 1: `8dc23861e8bf87345f20218c587244329e1caf6f`.
- Rama: `feature/tsse-testing`.
- Revision Git de Milestone 4: `54a91ed4b684b5ac6f4221c8743768fad5083270`; las ejecuciones formales tambien usaron inputs test/infraestructura dirty identificados por `evidencias/01_proyecto/milestone_4_20261015T161203Z/test_input_checksums.typescript`, por lo que HEAD solo no reproduce el run.
- Estado: ejecucion tecnica Milestone 4 reconciliada; ADS1263 66/66 PASS, acquisition 30/30 PASS (24/24 CFT), integracion host 71/71 PASS en dos runs, y analisis estatico no-gpiod completado. Hardware permanece fuera de alcance; libgpiod-v2 esta bloqueado; DEF-001 permanece abierto.

## Convenciones

Estados:

- `PASS`: actividad ejecutada con evidencia y resultado conforme.
- `FAIL`: actividad ejecutada con diferencia observada.
- `BLOCKED`: actividad en alcance sin herramienta, contrato o condicion habilitante.
- `NOT RUN`: caso disenado o planificado sin ejecucion.
- `OUT OF SCOPE`: actividad excluida deliberadamente.

Clasificacion de la afirmacion:

- `SOFTWARE CONTRACT`: verificable totalmente en software host dentro del limite declarado.
- `PARTIAL HOST/MOCKED`: verifica logica o interaccion simulada, no el fenomeno fisico.
- `REQUIRES HARDWARE`: necesita target/hardware para cerrar el requisito tal como esta escrito.
- `UNRESOLVED`: el criterio normativo requiere una decision.

## Requisitos originales

| ID | Clasificacion | Metodo/casos previstos | Estado actual | Evidencia o destino | Limite de la afirmacion |
|---|---|---|---|---|---|
| REQ-001 | `REQUIRES HARDWARE` | HW-LEGACY-001 | `OUT OF SCOPE` | `docs/acceptance_tests.md` | TSSE host no verifica ejecucion target |
| REQ-002 | `PARTIAL HOST/MOCKED` | UT-SPI-001-A/B; UT-SPI-002..004; INT-MOCK-001; INT-MOCK-CHECK; INT-STRACE-NO-DEVICE | Integracion `PASS`; unit adapters `NOT RUN` | `evidencias/05_integracion_host/milestone_4_20261015T161203Z/`; catalogo y suplemento canonico | Verifica proceso mock y ausencia observada de acceso a devices; no bus fisico ni ADS1263 |
| REQ-003 | `PARTIAL HOST/MOCKED` | UT-CFG-001..002; UT-SPI-005 | UT-CFG-001 `PASS`; restantes `NOT RUN` | `evidencias/04_unitarias/milestone_2a_20261015T142406Z/`; destinos en catalogo | Verifica valores y argumentos ioctl, no waveform |
| REQ-004 | `PARTIAL HOST/MOCKED` | UT-REG-001-A..D; UT-REG-002..006; UT-REG-009..011; UT-PRES-001..003; UT-PRES-006..011; CFT-P14..P15; HAL-HAPPY-SAMPLE | Unit/CFT/integracion `PASS` | evidencia Milestone 2B y Milestone 4; detalle en catalogo/suplemento | Verifica RREG/WREG/mapeo y respuesta simulada; lectura fisica sigue abierta |
| REQ-005 | `PARTIAL HOST/MOCKED` | UT-PRES-004..005; CFT-P12..P13; HAL-PRESENCE-FAILURE | Unit/CFT/integracion `PASS` | evidencia Milestone 2B y Milestone 4 | Verifica clasificacion simulada 0x00/0xFF; desconexion fisica queda fuera |
| REQ-006 | `PARTIAL HOST/MOCKED` | UT-CFG-001; UT-DRDY-001-A/B; UT-DRDY-002..007 | Unit `PASS` | evidencia Milestone 2A/2B/3A; detalle en catalogo | Verifica defaults/logica GPIO simulada, no PG8 fisico; ROB-003 sigue sin contrato fatal |
| REQ-007 | `PARTIAL HOST/MOCKED` | UT-CHAN-001..007; CFT-P07; CFT-P16..P17; CFT-P21; CLI-CHANNEL-* | Unit/CFT/integracion `PASS` | evidencia Milestone 2B y Milestone 4 | Verifica seleccion/secuencia simulada de canal 0, no señal analogica |
| REQ-008 | `SOFTWARE CONTRACT` | UT-SAMPLE-001-A/B; UT-SAMPLE-002..003; UT-ACQ-DSP017-001..002; PROC-DSP017-001; CFT-P17..P24; escenarios INT/HAL | Unit/CFT/integracion `PASS`; PROC-DSP017-001 `FAIL` | evidencia Milestone 3A y `evidencias/04_unitarias/milestone_4_20261015T161203Z/`; integracion Milestone 4 | DEF-001 persiste: fixed N>=2 con rate 1 valido no completa; no afirma rendimiento target |
| REQ-009 | `SOFTWARE CONTRACT` | INT-LIVE-001..002; INT-LIVE-SIGINT | Escenario ejecutado `PASS`; IDs legacy `NOT RUN` | suplemento canonico de integracion Milestone 4 | El escenario ejecutado usa rate 10 y corte tras al menos dos registros; no es evidencia exacta de los dos IDs legacy rate 2 |
| REQ-010 | `SOFTWARE CONTRACT` con DSP-005/006 | CFT-P18..P19; CFT-P24; INT-CSV-001..003; escenarios INT-CSV-* | CFT y escenarios ejecutados `PASS`; IDs legacy no identicos `NOT RUN` | evidencia unitaria/integracion Milestone 4 | Truncado, lifecycle y fallo no fatal quedan caracterizados; politica normativa de open failure sigue ambigua |
| REQ-011 | `SOFTWARE CONTRACT` | CFT-P01..P08; CLI legacy; registro ejecutado CLI | CFT `PASS`; 71-ID registry `PASS`; `CLI-004-D`, `CLI-005` y `CLI-006-F` `NOT RUN` | catalogo y `evidencias/08_trazabilidad/milestone_4_20261015T161203Z/integration_case_traceability.md` | “Claro” se operacionaliza como retorno y fragmento esperado; `CLI-LIVE-CSV` no tiene el input exacto de CLI-006-F |
| REQ-012 | `UNRESOLVED` | ARCH-001 | `BLOCKED` | `docs/tsse/discrepancias.md#dsp-003` | Ubicacion se puede inspeccionar; abstraccion completa no esta definida |

Ningun `NOT RUN` prueba cumplimiento. La columna de casos distingue los IDs pendientes de los CFT y escenarios que ya tienen evidencia observada.

El inventario legacy de integracion queda en `19 PASS` y `10 NOT RUN`. Los diez `NOT RUN` son `INT-ACQ-001`, `INT-ACQ-002`, `INT-CSV-001`, `INT-CSV-002`, `INT-CSV-003`, `INT-LIVE-001`, `INT-LIVE-002`, `CLI-004-D`, `CLI-005` y `CLI-006-F`.

## Requisitos de infraestructura de testing

Estos requisitos son nuevos para el proceso TSSE; no modifican REQ-001..REQ-012.

| ID | Requisito de testing | Casos atomicos | Estado | Evidencia actual |
|---|---|---|---|---|
| TST-001 | Preservar baseline, revision, entorno y checksums | AUD-BASE-001 | `PASS` | baseline M1; `evidencias/01_proyecto/milestone_4_20261015T161203Z/README.md` y `test_input_checksums.typescript` para M4 |
| TST-002 | Mantener trazabilidad atomica y estados normalizados | REV-MTP-001; REV-CFT-002 | `PASS` | `docs/tsse/catalogo_casos.md`; `evidencias/01_proyecto/milestone_1_revision_trazabilidad.md` |
| TST-003 | Modelar CFT de componente con decisiones, edges y factibilidad | REV-CFT-001..002; CFT-P01..P24 | `PASS` | `docs/tsse/cft_acquisition_run_samples.md`; evidencia de revision |
| TST-004 | Compilar fuente de produccion real en unit tests | AUD-SOURCE-001 | `PASS` | `ads1263.c` y `acquisition.c` reales en `evidencias/04_unitarias/milestone_3a_20261015T150616Z/` |
| TST-005 | Ejecutar unit tests deterministas sin hardware/esperas reales | RUN-DET-001 | `PASS` | Milestone 4: ADS1263 66/66 y acquisition 30/30; fallo link inicial preservado y rerun limpio |
| TST-006 | Ejecutar integracion host con limites simulados declarados | 71 escenarios ejecutados; IDs legacy relacionados | `PASS` para registry ejecutado | dos runs independientes 71/71, 0 fail/blocked; suplemento canonico Milestone 4 |
| TST-007 | Medir line/branch/function coverage por modulo sin umbral inventado | ANA-COV-001 | `PASS` | `evidencias/06_cobertura/milestone_4_20261015T161203Z/` |
| TST-008 | Ejecutar analisis estatico reproducible por configuracion | ANA-STATIC-001..002 | no-gpiod `PASS`; libgpiod-v2 `BLOCKED` | `evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/` |
| TST-009 | Preservar evidencia con provenance y distinguir expected/observed | AUD-EVID-001 | `PASS` | auditorias y manifest Milestone 4 |
| TST-010 | Exigir gate antes de modificar produccion | AUD-PROD-001 | `PASS` | `evidencias/01_proyecto/milestone_1_guarda_produccion.md` |

## Hipotesis de robustez

| ID | Objetivo | Casos previstos | Estado | Defecto confirmado | Restriccion |
|---|---|---|---|---|---|
| ROB-001 | Caracterizar rango combinado de registros | UT-REG-007..008 + ASan/UBSan | `PASS` caracterizacion | No; `FND-ROB001-001` sospechado | Efecto intraobjeto reproducido; contrato de rechazo pendiente; sanitizer PASS no prueba seguridad |
| ROB-002 | Verificar tratamiento de fallas SPI | UT-REG-005/010; UT-PRES-003; UT-CHAN-003/004/006; UT-SAMPLE-008; UT-SPI-004; CFT-P20/P22/P24; HAL-READ-FAILURE | Core/CFT/HAL simulado `PASS`; adapter unit `NOT RUN` | No | Dependencias simuladas; snapshot zeroed de UT-PRES-003 es observacion, no defecto |
| ROB-003 | Caracterizar propagacion DRDY | UT-DRDY-004..007; UT-ACQ-ROB003-001..004 | `PASS` caracterizacion | No | Cuatro combinaciones y consecuencias separadas; no existe contrato aprobado de retorno fatal |
| ROB-004 | Verificar conversion ADC en limites | UT-VOLT-001; UT-VOLT-002-A/B; UT-VOLT-003-A/B; UT-VOLT-004-A/B; UT-VOLT-005; UT-SAMPLE-004..007 | `PASS` | No | Resultado numerico host, no exactitud ADC fisica; evidencia Milestone 2A/2B |
| DSP-017 | Caracterizar pacing rate 1 | UT-ACQ-DSP017-001..002; PROC-DSP017-001 | Unit `PASS`; proceso `FAIL` | Si, DEF-001 | Entrada valida N=2/rate=1 no completa por retry EINVAL; correccion requiere gate humano |

## Inventario atomico

El inventario de casos planificados se mantiene en `docs/tsse/catalogo_casos.md`; los CFT detallados estan en `docs/tsse/cft_cases.md`. El registro completo de los 71 IDs de integracion realmente ejecutados es el suplemento canonico `docs/tsse/evidencias/08_trazabilidad/milestone_4_20261015T161203Z/integration_case_traceability.md`.

## Reglas de cierre

- Cada caso debe enlazar un requisito original, TST o ROB.
- Cada afirmacion cuantitativa debe enlazar evidencia de ejecucion.
- Los casos mock limitan su afirmacion al contrato software.
- Los estados solo cambian de `NOT RUN` cuando existe evidencia real.
- `REQ-001` permanece `OUT OF SCOPE`; `REQ-012` permanece `BLOCKED` hasta decision del criterio.
- Los ROB no se convierten en requisitos originales ni defectos por documentacion sola.
