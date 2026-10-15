# Inventario final de estados - Milestone 4

| Area | PASS | FAIL | BLOCKED | NOT RUN / abierto | Nota |
|---|---:|---:|---:|---:|---|
| Unitarios ADS1263 no CFT | 66 | 0 | 0 | 0 | Intento link inicial preservado como infraestructura |
| Unitarios acquisition no CFT | 6 | 0 | 0 | 0 | Robustez/caracterizacion; junto con ADS1263 forman 72 no CFT |
| CFT-P01..P24 | 24 | 0 | 0 | 0 | Inventario separado; la suite acquisition totaliza 30 |
| Integracion run 1 | 71 | 0 | 0 | 0 | Inventario declarado=ejecutado |
| Integracion run 2 | 71 | 0 | 0 | 0 | Mismos IDs/estados que run 1 |
| Casos legacy de integracion del catalogo | 19 | 0 | 0 | 10 | Solo PASS con input/oracle exactos |
| Unitarios legacy de config/adapters | 0 | 0 | 0 | 7 | No ejecutados en Milestone 4 |
| Analisis estatico no-gpiod | 1 actividad con findings | 0 | 0 | 0 | 41 style, cero defectos confirmados |
| Analisis estatico libgpiod-v2 | 0 | 0 | 1 | 0 | Faltan `pkg-config` y `gpiod.h` |
| Cobertura por modulo | 2 | 0 | 0 | 0 | Sin umbral de aceptacion |
| Defectos de produccion | 0 cerrados | 1 abierto (DEF-001) | 0 | decisiones contractuales abiertas | Sin correccion |
| Aprobacion humana de reporte/rehearsal | 0 | 0 | 2 | 0 | `PENDING HUMAN`; no se marca completa |

Los diez IDs legacy de integracion `NOT RUN` son `INT-ACQ-001`, `INT-ACQ-002`, `INT-CSV-001`, `INT-CSV-002`, `INT-CSV-003`, `INT-LIVE-001`, `INT-LIVE-002`, `CLI-004-D`, `CLI-005` y `CLI-006-F`. Los siete unitarios legacy `NOT RUN` son `UT-CFG-002`, `UT-SPI-001-A`, `UT-SPI-001-B`, `UT-SPI-002`, `UT-SPI-003`, `UT-SPI-004` y `UT-SPI-005`.

Los estados `BLOCKED` abiertos son ANA-STATIC-002 para libgpiod-v2 por falta de `pkg-config`/`gpiod.h`, ARCH-001/REQ-012 por falta de criterio contractual de aislamiento, la aprobacion factual del informe pendiente de revisor humano y el rehearsal pendiente de entrega humana. DEF-001 permanece abierto; ROB-001 sigue sospechado; ROB-003 y las decisiones contractuales de CSV/DRDY permanecen abiertas.

Los 71 PASS de cada run no se suman como 142 IDs unicos: ambos runs contienen los mismos 71 IDs con los mismos 71 estados `PASS`. Hardware, target, SPI/GPIO fisicos, ADC y tiempo real siguen fuera del alcance de la evidencia host.
