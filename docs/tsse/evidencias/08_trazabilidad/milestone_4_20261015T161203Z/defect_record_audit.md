# Auditoria de defectos - Milestone 4

| Registro | Estado preservado | Evidencia/disposicion |
|---|---|---|
| DEF-001 / DSP-017 | Confirmado, abierto, sin correccion | `07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md`; los unitarios relacionados siguen PASS, sin convertir el fallo de proceso en PASS |
| ROB-001 / FND-ROB001-001 | Sospechado, no defecto confirmado | Efecto intraobjeto reproducido; falta contrato de rechazo |
| ROB-002 | PASS parcial ampliado | Core, CFT-P20/P22/P24 y HAL-READ-FAILURE; adapters unitarios pendientes; sin defecto |
| ROB-003 | PASS caracterizacion, ambiguedad contractual | Cuatro combinaciones unitarias preservadas; no se exige retorno fatal sin decision |
| ROB-004 | PASS host numerico | No prueba exactitud ADC fisica |
| Analisis estatico | Cero defectos confirmados | 41 style: 38 falsos positivos de contexto/herramienta y tres observaciones aceptadas de interfaz publica |
| DSP-005..DSP-008 | Caracterizacion de integracion | Truncado CSV, open failure no fatal, orden HELP/modos y CSV con check observados; ninguna ambiguedad se convierte en defecto |

El fallo link inicial de unitarios y la carrera gcovr son incidentes de infraestructura preservados, no defectos de produccion. No hubo cambio de produccion y DEF-001 no se cierra.
