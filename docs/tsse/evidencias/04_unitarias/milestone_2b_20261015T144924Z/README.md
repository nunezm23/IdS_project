# Unit tests extendidos ADS1263 - Milestone 2B

## Alcance ejecutado

- Suites: `test_ads1263_initial.c`, `test_ads1263_registers.c`, `test_ads1263_presence.c`, `test_ads1263_channel.c` y `test_ads1263_sample.c`.
- SUT real: `components/ads1263/src/ads1263.c`.
- Dependencias: SPI/GPIO mockeados con CMock; `nanosleep` interpuesto sin espera real; hardware no usado.
- Inventario total: 62 tests, formado por 9 iniciales, 20 de registros, 11 de presencia, 7 de canal y 15 de muestra/DRDY.
- Metadata comun: `docs/tsse/evidencias/01_proyecto/milestone_2b_20261015T144924Z/README.md`.

## Comandos y resultados

| Actividad | Comando exacto | Inicio observado | Exit | Evidencia raw |
|---|---|---|---:|---|
| Run 1 limpio | `ceedling clobber`; `ceedling test:all` | 2026-10-15 09:49:33 -05:00 | 0; 0 | `01_clobber_run_1.typescript`; `02_unit_run_1.typescript` |
| Run 2 limpio | `ceedling clobber`; `ceedling test:all` | 2026-10-15 09:49:42 -05:00 | 0; 0 | `03_clobber_run_2.typescript`; `04_unit_run_2.typescript` |

Cada run limpio reconstruyo y ejecuto el mismo inventario: 62 tests, 62 `PASS`, 0 `FAIL`, 0 `IGNORED`. Los resultados machine-readable del segundo run son `tests_report.json` y `junit_tests_report.xml`.

## Resultados atomicos nuevos

| Casos | Cantidad de tests | Resultado | Oracle observado |
|---|---:|---|---|
| UT-REG-001-A..D | 4 | `PASS` | Nulls rechazados sin modificar buffers de salida aplicables |
| UT-REG-002..003 | 2 | `PASS` | RREG exacto para registros inicial y final, con dato retornado |
| UT-REG-004-A..D | 4 | `PASS` | Limites de `count` y `start`; lectura completa de 32 registros |
| UT-REG-005 | 1 | `PASS` | Fallo SPI propagado y output preservado |
| UT-REG-006-A..B | 2 | `PASS` | WREG exacto para registros inicial y final |
| UT-REG-009-A..D | 4 | `PASS` | Limites de `count` y `start`; escritura completa de 32 registros |
| UT-REG-010 | 1 | `PASS` | Fallo SPI de escritura propagado |
| UT-REG-011-A..B | 2 | `PASS` | Read/write mock usan `mock_registers` sin transferencia SPI |
| UT-PRES-001..011 | 11 | `PASS` | Argumentos, frame RREG, SPI, clasificaciones zero/FF, mismatches y snapshots validos |
| UT-CHAN-001..007 | 7 | `PASS` | Argumentos, secuencia WREG/RREG/START1 y supresion posterior a errores |
| UT-SAMPLE-001-A..B | 2 | `PASS` | Nulls rechazados sin waits |
| UT-SAMPLE-002..008 | 7 | `PASS` | Mock determinista, RDATA1, signed decode, extremos, metadata y fallo SPI |
| UT-DRDY-001-A..B | 2 | `PASS` | Nulls de lectura DRDY rechazados |
| UT-DRDY-002..003 | 2 | `PASS` | Ready inmediato y secuencia busy/busy/ready con orden GPIO/wait/SPI |
| UT-DRDY-004..005 | 2 | `PASS` | Error GPIO y timeout quedan en la muestra; SPI exitoso determina retorno OK |

Los nueve casos iniciales de Milestone 2A tambien volvieron a pasar, pero su evidencia historica y su clasificacion no se sustituyen.

## Observaciones de caracterizacion

- UT-PRES-003 observo que un fallo SPI deja el snapshot en cero porque `ads1263_check_presence()` lo limpia antes de transferir. No existe un contrato que clasifique esto como defecto.
- UT-DRDY-004 observo retorno `ADS1263_OK` tras fallo GPIO si RDATA1 funciona, con `sample.drdy=-1` y `sample.status="GPIO_READ"`.
- UT-DRDY-005 observo retorno `ADS1263_OK` tras 1000 polls busy si RDATA1 funciona, con `sample.drdy=1` y `sample.status="DRDY_TIMEOUT"`.
- El timeout uso exactamente 1000 llamadas al wrapper de espera y no espero tiempo real.
- Los dos ultimos resultados son evidencia de ROB-003/DSP-011 y mantienen la decision contractual pendiente; no se registran como defectos confirmados.

## Limite de la afirmacion

UT-REG-007/008 (rangos que cruzan el final), UT-DRDY-006/007 (DRDY adverso junto con fallo SPI), consecuencias acquisition ROB-003, acquisition general, adapters, ROB-002, CFT e integracion no fueron ejecutados en este milestone. Permanecen `NOT RUN`; no se infiere su resultado a partir de cobertura.
