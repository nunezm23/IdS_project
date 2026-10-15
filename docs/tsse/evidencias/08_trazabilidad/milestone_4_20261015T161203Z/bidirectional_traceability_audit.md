# Auditoria de trazabilidad bidireccional - Milestone 4

## Forward: fuente a evidencia

| Fuente | Estado/resultado | Registro de casos |
|---|---|---|
| REQ-001 | `OUT OF SCOPE` | Hardware legacy, sin cierre host |
| REQ-002..REQ-008 | Parcial host/mock; casos unitarios/CFT/integracion ejecutados segun matriz | `catalogo_casos.md`, `cft_cases.md`, suplemento de integracion |
| REQ-009 | Escenario live ejecutado `PASS`; IDs legacy rate 2 permanecen `NOT RUN` | suplemento y catalogo |
| REQ-010 | CFT y escenarios CSV `PASS`; politica de open failure ambigua | suplemento, catalogo, DSP-005/006 |
| REQ-011 | CFT de validacion y registry CLI ejecutados; `CLI-004-D`, `CLI-005` y `CLI-006-F` no tienen ejecucion exacta y siguen `NOT RUN` | catalogo y suplemento |
| REQ-012 | `BLOCKED` por criterio contractual | ARCH-001, DSP-003 |
| TST-001..TST-010 | Estados en matriz; TST-006/008/009 actualizados por Milestone 4 | matriz, catalogo y manifest |
| ROB-001..ROB-004 | Estados preservados; ROB-002 amplia CFT/HAL simulado; no se crean defectos | matriz y auditoria de defectos |

## Reverse: evidencia a fuente

| Evidencia ejecutada | Fuente trazada |
|---|---|
| 66 unitarios ADS1263 | REQ-003..REQ-008, ROB-001..ROB-004, TST-004/005 |
| 24 CFT + 6 acquisition no CFT | REQ-004/005/007/008/010/011, ROB-002/003, TST-003/005 |
| 71 escenarios host | REQ/TST/ROB por fila en `integration_case_traceability.md` |
| Cobertura | TST-007 / ANA-COV-001 |
| Analisis estatico | TST-008 / ANA-STATIC-001/002 |
| Integridad y resumen | TST-001/009/010 / AUD-BASE-001, AUD-EVID-001, AUD-PROD-001 |

Resultado: no hay ID ejecutado sin fuente en el registro canonico. Permanecen abiertos REQ-012, la configuracion libgpiod-v2, diez casos legacy de integracion con inputs no identicos y las decisiones contractuales enumeradas en `discrepancias.md`.
