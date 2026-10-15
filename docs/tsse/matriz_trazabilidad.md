# Matriz de trazabilidad - TSSE

## Referencia

- Proyecto: `IdS_project`.
- Baseline de produccion: `c59b24017f5daebb4e37512d799a0d07f589bce5`.
- Revision documental al iniciar Milestone 1: `8dc23861e8bf87345f20218c587244329e1caf6f`.
- Rama: `feature/tsse-testing`.
- Estado: Milestone 2B; infraestructura Ceedling activa y 62 tests ejecutados en dos runs limpios, con acquisition, adapters, robustez restante e integracion aun diferidos.

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
| REQ-002 | `PARTIAL HOST/MOCKED` | UT-SPI-001-A/B; UT-SPI-002..004; INT-MOCK-001 | `NOT RUN` | futura evidencia unitaria/integracion | Verifica path/default/tramas, no bus fisico ni ADS1263 |
| REQ-003 | `PARTIAL HOST/MOCKED` | UT-CFG-001..002; UT-SPI-005 | UT-CFG-001 `PASS`; restantes `NOT RUN` | `evidencias/04_unitarias/milestone_2a_20261015T142406Z/`; destinos en catalogo | Verifica valores y argumentos ioctl, no waveform |
| REQ-004 | `PARTIAL HOST/MOCKED` | UT-REG-001-A..D; UT-REG-002..006; UT-REG-009..011; UT-PRES-001..003; UT-PRES-006..011; CFT-P14..P15 | Unit `PASS`; CFT `NOT RUN` | `evidencias/04_unitarias/milestone_2b_20261015T144924Z/`; `docs/tsse/cft_cases.md` | Verifica RREG/WREG/mapeo simulado; respuesta acquisition y lectura fisica siguen abiertas |
| REQ-005 | `PARTIAL HOST/MOCKED` | UT-PRES-004..005; CFT-P12..P13 | Unit `PASS`; CFT `NOT RUN` | `evidencias/04_unitarias/milestone_2b_20261015T144924Z/`; `docs/tsse/cft_cases.md` | Verifica clasificacion simulada 0x00/0xFF; desconexion fisica queda fuera |
| REQ-006 | `PARTIAL HOST/MOCKED` | UT-CFG-001; UT-DRDY-001-A/B; UT-DRDY-002..007 | UT-CFG-001 y UT-DRDY-001..005 `PASS`; UT-DRDY-006..007 `NOT RUN` | evidencia Milestone 2A/2B; detalle en catalogo | Verifica defaults/logica GPIO simulada, no PG8 fisico; ROB-003 sigue sin contrato fatal |
| REQ-007 | `PARTIAL HOST/MOCKED` | UT-CHAN-001..007; CFT-P07; CFT-P16..P17; CFT-P21 | Unit `PASS`; CFT `NOT RUN` | `evidencias/04_unitarias/milestone_2b_20261015T144924Z/`; `docs/tsse/cft_cases.md` | Verifica seleccion/secuencia simulada de canal 0, no señal analogica |
| REQ-008 | `SOFTWARE CONTRACT` | UT-SAMPLE-001-A/B; UT-SAMPLE-002..003; CFT-P17..P24; INT-ACQ-001..004 | Unit `PASS`; CFT/integracion `NOT RUN` | `evidencias/04_unitarias/milestone_2b_20261015T144924Z/`; `docs/tsse/cft_cases.md` | Verifica lectura/parsing unitario; conteo/control de acquisition y frecuencia real siguen abiertos |
| REQ-009 | `SOFTWARE CONTRACT` | INT-LIVE-001..002 | `NOT RUN` | futura evidencia integracion host | No forma parte del CFT fijo ni de Milestone 1 |
| REQ-010 | `SOFTWARE CONTRACT` con DSP-006 | CFT-P18..P19; CFT-P24; INT-CSV-001..003 | `NOT RUN` | `docs/tsse/cft_cases.md` y futura evidencia | Exito CSV verificable; politica de open failure sigue sin contrato final |
| REQ-011 | `SOFTWARE CONTRACT` | CFT-P01..P08; INT-ACQ-003; CLI-001..002; CLI-003-A..F; CLI-004-A..D; CLI-005; CLI-006-A..F | `NOT RUN` | `docs/tsse/cft_cases.md` y futura evidencia | “Claro” se operacionaliza como retorno y fragmento esperado |
| REQ-012 | `UNRESOLVED` | ARCH-001 | `BLOCKED` | `docs/tsse/discrepancias.md#dsp-003` | Ubicacion se puede inspeccionar; abstraccion completa no esta definida |

Ningun `NOT RUN` prueba cumplimiento. La columna de casos describe diseno futuro o casos CFT documentados.

## Requisitos de infraestructura de testing

Estos requisitos son nuevos para el proceso TSSE; no modifican REQ-001..REQ-012.

| ID | Requisito de testing | Casos atomicos | Estado | Evidencia actual |
|---|---|---|---|---|
| TST-001 | Preservar baseline, revision, entorno y checksums | AUD-BASE-001 | `PASS` | `evidencias/01_proyecto/milestone_1_linea_base.md` |
| TST-002 | Mantener trazabilidad atomica y estados normalizados | REV-MTP-001; REV-CFT-002 | `PASS` | `docs/tsse/catalogo_casos.md`; `evidencias/01_proyecto/milestone_1_revision_trazabilidad.md` |
| TST-003 | Modelar CFT de componente con decisiones, edges y factibilidad | REV-CFT-001..002; CFT-P01..P24 | `PASS` | `docs/tsse/cft_acquisition_run_samples.md`; evidencia de revision |
| TST-004 | Compilar fuente de produccion real en unit tests | AUD-SOURCE-001 | `PASS` | suites en `evidencias/04_unitarias/milestone_2b_20261015T144924Z/`; inventario en `evidencias/01_proyecto/milestone_2b_20261015T144924Z/` |
| TST-005 | Ejecutar unit tests deterministas sin hardware/esperas reales | RUN-DET-001 | `PASS` | dos runs limpios 62/62 en `evidencias/04_unitarias/milestone_2b_20261015T144924Z/` |
| TST-006 | Ejecutar integracion host con limites simulados declarados | INT-MOCK-001; INT-ACQ-001..004; INT-CSV-001..003; INT-LIVE-001..002; CLI-001..002; CLI-003-A..F; CLI-004-A..D; CLI-005; CLI-006-A..F | `NOT RUN` | Milestone 3 |
| TST-007 | Medir line/branch/function coverage por modulo sin umbral inventado | ANA-COV-001 | `PASS` | baseline preservado y comparacion en `evidencias/06_cobertura/milestone_2b_20261015T144924Z/` |
| TST-008 | Ejecutar analisis estatico reproducible por configuracion | ANA-STATIC-001..002 | `NOT RUN` | Milestone 4 |
| TST-009 | Preservar evidencia con provenance y distinguir expected/observed | AUD-EVID-001 | `NOT RUN` | Cierre futuro; politica en MTP |
| TST-010 | Exigir gate antes de modificar produccion | AUD-PROD-001 | `PASS` | `evidencias/01_proyecto/milestone_1_guarda_produccion.md` |

## Hipotesis de robustez

| ID | Objetivo | Casos previstos | Estado | Defecto confirmado | Restriccion |
|---|---|---|---|---|---|
| ROB-001 | Caracterizar rango combinado de registros | UT-REG-007..008 + ASan | `NOT RUN` | No | No corregir ni asumir rechazo antes de evidencia/contrato |
| ROB-002 | Verificar tratamiento de fallas SPI | UT-REG-005/010; UT-PRES-003; UT-CHAN-003/004/006; UT-SAMPLE-008; UT-SPI-004; CFT-P20/P22/P24 | Core `PASS`; adapter/CFT `NOT RUN` | No | Dependencias simuladas; snapshot zeroed de UT-PRES-003 es observacion, no defecto |
| ROB-003 | Caracterizar propagacion DRDY | UT-DRDY-004..007; UT-ACQ-ROB003-001..004 | UT-DRDY-004..005 `PASS`; restantes `NOT RUN` | No | SPI exitoso produce retorno OK y estado diagnostico; no existe contrato aprobado de retorno fatal |
| ROB-004 | Verificar conversion ADC en limites | UT-VOLT-001; UT-VOLT-002-A/B; UT-VOLT-003-A/B; UT-VOLT-004-A/B; UT-VOLT-005; UT-SAMPLE-004..007 | `PASS` | No | Resultado numerico host, no exactitud ADC fisica; evidencia Milestone 2A/2B |

## Inventario atomico

El inventario completo de casos, incluyendo precondiciones, inputs, oraculo, estado, evidencia y defecto, se mantiene en `docs/tsse/catalogo_casos.md`. Los casos CFT detallados y sus secuencias se mantienen en `docs/tsse/cft_cases.md`.

## Reglas de cierre

- Cada caso debe enlazar un requisito original, TST o ROB.
- Cada afirmacion cuantitativa debe enlazar evidencia de ejecucion.
- Los casos mock limitan su afirmacion al contrato software.
- Los estados solo cambian de `NOT RUN` cuando existe evidencia real.
- `REQ-001` permanece `OUT OF SCOPE`; `REQ-012` permanece `BLOCKED` hasta decision del criterio.
- Los ROB no se convierten en requisitos originales ni defectos por documentacion sola.
