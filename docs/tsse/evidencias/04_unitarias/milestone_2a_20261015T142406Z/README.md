# Unit tests iniciales ADS1263 - Milestone 2A

## Alcance ejecutado

- Suite: `test/ceedling/tests/test_ads1263_initial.c`.
- SUT real: `components/ads1263/src/ads1263.c`.
- Funciones seleccionadas: `ads1263_default_config()` y `ads1263_raw_to_voltage()`.
- Tolerancia absoluta de conversion: `1e-12`.
- Dependencias: SPI/GPIO mockeados con CMock; espera interpuesta; hardware no usado.
- Metadata comun: `docs/tsse/evidencias/01_proyecto/milestone_2a_20261015T142406Z/README.md`.

## Comandos y resultados

| Actividad | Comando exacto | Inicio observado | Exit | Evidencia raw |
|---|---|---|---:|---|
| Run 1 limpio | `ceedling clobber`; `ceedling test:all` | 2026-10-15 09:24:45 -05:00 | 0 | `03_clobber_after_unity_fix.typescript`; `04_unit_run_1_pass.typescript` |
| Run 2 limpio | `ceedling clobber`; `ceedling test:all` | 2026-10-15 09:24:51 -05:00 | 0 | `05_clobber_before_run_2.typescript`; `06_unit_run_2_pass.typescript` |
| Probe de exit no-cero | `ceedling test:all` con una asercion temporal intencional | 2026-10-15 09:25:02 -05:00 | 1 | `07_intentional_failure_exit_probe.typescript` |
| Estado final limpio | `ceedling clobber`; `ceedling test:all` | registrado en transcript | 0 | `08_clobber_after_exit_probe.typescript`; `09_final_unit_run_pass.typescript` |

Los dos runs limpios ejecutaron el mismo inventario: 9 tests, 9 `PASS`, 0 `FAIL`, 0 `IGNORED`. El probe temporal ejecuto 10 tests, produjo exactamente un fallo intencional y retorno 1; su archivo fue eliminado antes del run final. Los resultados finales machine-readable son `tests_report.json` y `junit_tests_report.xml`.

## Resultados atomicos

| ID | Resultado | Oracle observado |
|---|---|---|
| UT-CFG-001 | `PASS` | Paths, linea 8, 1000000 Hz, 2.5 V, gain 1, DRDY activo y mock inactivo |
| UT-VOLT-001 | `PASS` | raw 0 produce 0 V |
| UT-VOLT-002-A | `PASS` | raw representativo positivo cumple formula y signo |
| UT-VOLT-002-B | `PASS` | raw representativo negativo cumple formula y signo |
| UT-VOLT-003-A | `PASS` | `INT32_MAX` produce 2.5 V finitos |
| UT-VOLT-003-B | `PASS` | `INT32_MIN` cumple formula signed y produce valor finito |
| UT-VOLT-004-A | `PASS` | gain 2 aplica escala esperada |
| UT-VOLT-004-B | `PASS` | Vref 5.0 aplica escala esperada |
| UT-VOLT-005 | `PASS` | gain 0 reproduce el comportamiento actual de gain 1 |

UT-VOLT-005 es caracterizacion del comportamiento actual, no aprobacion de un contrato de gain cero ni correccion de produccion.

## Incidencias de infraestructura preservadas

El primer intento (`02_unit_run_1.typescript`) compilo y enlazo el SUT pero los nueve tests fallaron porque Unity 2.7.2 requiere `UNITY_INCLUDE_DOUBLE` para aserciones double. Se clasifico como configuracion de infraestructura, se agrego el define al perfil de tests y se conservaron el fallo y los passes posteriores. No se cambio produccion y no se registro defecto de producto.

## Limite de la afirmacion

Solo estas nueve pruebas iniciales fueron implementadas y ejecutadas. Registro, presencia, canal, muestra, DRDY, acquisition, adapters, ROB-001/002/003, CFT e integracion permanecen `NOT RUN` o en su estado previo.
