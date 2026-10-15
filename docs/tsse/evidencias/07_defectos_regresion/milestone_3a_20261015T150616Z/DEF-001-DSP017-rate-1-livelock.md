# DEF-001 - Livelock con fixed samples y rate_hz=1

## Clasificacion

- Estado: `CONFIRMED DEFECT`.
- Disposicion: abierto; correccion no autorizada en Milestone 3A.
- Fuente: `components/acquisition/src/acquisition.c:154-166,245-250`.
- Contrato: REQ-008 exige fixed sample count verificable con exactamente N muestras; `main/main.c:125-130` y `acquisition.c:141-143` aceptan `rate_hz` en `1..10`.

## Preconditions y reproduccion minima

- Host Ubuntu 26.04.1 x86-64, glibc/POSIX `nanosleep`.
- `samples=2`, `rate_hz=1`, `channel=0`, sin CSV.
- ADS1263 y logger sustituidos por fakes deterministas; `acquisition.c` real.
- Build: comando exacto en `02_dsp017_probe_build_pass.typescript`.
- Ejecucion acotada: `timeout --signal=TERM --kill-after=1s 2s build/robustness/dsp017_rate1_probe`.

## Expected y actual

- Expected: al ser parametros aceptados, fixed mode retorna y produce exactamente dos muestras, o la entrada se rechaza explicitamente antes de iniciar.
- Actual: tras la primera muestra, `sleep_for_rate(1)` construye `{tv_sec=0,tv_nsec=1000000000}`. POSIX lo rechaza con `EINVAL`; el `while` repite el mismo valor y no progresa. El probe termina solo por timeout externo con exit 124.

## Impacto

Fixed mode con `N>=2` y rate 1 no completa. Live mode con rate 1 queda sujeto al mismo retry despues de la primera muestra. El problema afecta una entrada declarada valida; no es una medicion de precision temporal ni una afirmacion de hardware.

## Evidencia

- `03_dsp017_probe_timeout.typescript`: reproduccion real acotada, exit 124.
- `../../04_unitarias/milestone_3a_20261015T150616Z/final/08_acquisition_test_run_1.typescript`: wrapper captura el timespec y POSIX host retorna EINVAL.
- `../../04_unitarias/milestone_3a_20261015T150616Z/final/10_acquisition_test_run_2.typescript`: repeticion limpia equivalente.
- `README.md`: tabla de investigacion y limites.

## Regresion futura

Una correccion separadamente aprobada debera conservar este before, agregar un caso real acotado que termine para N=2/rate=1, rerun de ambos proyectos Ceedling, coverage y guarda de produccion. No existe after en Milestone 3A.
