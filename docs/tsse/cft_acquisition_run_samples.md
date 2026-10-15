# Modelo CFT formal - `acquisition_run_samples()`

## Estado y provenance

- Tipo: diseno formal; no es resultado de ejecucion.
- Baseline de fuente modelada: `c59b24017f5daebb4e37512d799a0d07f589bce5`.
- Archivo: `components/acquisition/src/acquisition.c`.
- SHA-256: `0c37acc3f8283c507a9cd981878e3891a428b9e6fdb9f5457033dd0e2418ad06`.
- Revision documental de inicio: `8dc23861e8bf87345f20218c587244329e1caf6f`.
- Herramienta de render: Graphviz `dot` 14.1.2.
- Artefacto fuente: `docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.dot`.
- Renders: `acquisition_run_samples_cfg.svg` y `acquisition_run_samples_cfg.png` en el mismo directorio.

## Profundidad y frontera

Se usa profundidad 1 de componente: la raiz publica es `acquisition_run_samples()` y se expanden todos los helpers locales de `acquisition.c` alcanzables por ese entry point. Esta definicion evita dos errores:

- Analizar solo el cuerpo top-level omitiria casi todas las decisiones relevantes.
- Expandir ADS1263 y los adaptadores Linux mezclaria niveles de test y produciria un modelo desproporcionado.

### Funciones incluidas

| Funcion | Lineas | Razon |
|---|---:|---|
| `acquisition_run_samples` | 260-283 | Raiz CFT |
| `validate_acquisition_config` | 127-152 | Validacion y acumulacion de errores |
| `prepare_adc_for_acquisition` | 187-217 | Init, presencia, canal y cleanup |
| `log_init_error` | 67-82 | Decisiones de diagnostico de init |
| `print_snapshot` | 26-37 | Decision null y salida snapshot |
| `log_presence_error` | 39-65 | Clasificacion del error de presencia |
| `acquire_loop` | 219-258 | Loop, read, break, salida y cierre CSV |
| `open_csv_if_requested` | 168-185 | No solicitado, fallo y exito de apertura |
| `sleep_for_rate` | 154-166 | Rate cero y retry de `nanosleep` |

### Contratos externos

| Contrato | Outcomes modelados | No expandido |
|---|---|---|
| `ads1263_init` | OK, SPI_OPEN, GPIO_OPEN, otro error | Apertura/configuracion HAL |
| `ads1263_check_presence` | OK, ZERO, FLOATING, MISMATCH, otro error | RREG y clasificacion interna |
| `ads1263_configure_channel` | OK o error | WREG/RREG/START1 internos |
| `ads1263_read_sample` | OK con muestra inicializada o error | DRDY/RDATA1/parseo internos |
| `ads1263_status_to_string` | texto de diagnostico para el status recibido | Mapeo interno de status; call sites P03, P12, P15 y L06 |
| `ads1263_close` | llamada de cleanup sin retorno inspeccionado | STOP/liberacion HAL interna; call sites P13, P15 y N90 |
| Logger/`printf` | Evento observable | Formateo interno |
| `fopen` | stream o NULL | Filesystem interno |
| `fputs`/`fclose` | llamada observable; sus retornos no se inspeccionan | Buffering/persistencia |
| `nanosleep` | 0 o no-cero | Scheduler/reloj real |

Fuera del grafo: `main`, `acquisition_run_live`, handler SIGINT, ADS1263 core, HAL SPI/GPIO y hardware.

## Convencion de decisiones

Cada condicion atomica con capacidad de desviar control es un diamante binario. `||` y `&&` conservan short-circuit como decisiones separadas. El `switch` de presencia se normaliza como tres comparaciones binarias ordenadas. El ternario de retorno del validator cuenta como decision. Los outcomes externos se convierten en comparaciones dentro de acquisition, no en paths internos de la dependencia.

## Tabla de decisiones y edge list binaria

| ID | Predicado | Fuente | T -> | F -> | Factibilidad desde la raiz fija |
|---|---|---:|---|---|---|
| D01 | `adc_config == NULL` | 266 | N92 | V00 | Ambas |
| D02 | validator result `!= 0` | 266 | N92 | N01 | Ambas, condicionadas por D03-D09 |
| D03 | `config == NULL` | 131 | V01 | D04 | Ambas si D01=F |
| D04 | `samples < 1` | 136 | V02 | D05 | Ambas; unsigned implica cero |
| D05 | `samples > 1000` | 136 | V02 | D06 | Ambas; solo si D04=F |
| D06 | `rate_hz < 1` | 141 | V03 | D07 | Ambas; unsigned implica cero |
| D07 | `rate_hz > 10` | 141 | V03 | D08 | Ambas; solo si D06=F |
| D08 | `channel != 0` | 146 | V04 | D09 | Ambas |
| D09 | `errors == 0` | 151 | V05 | V06 | Ambas, determinada por D04-D08 |
| D10 | init status `!= OK` | 195 | D11 | P05 | Ambas |
| D11 | status `== SPI_OPEN` | 69 | P01 | D12 | Ambas en rama error init |
| D12 | status `== GPIO_OPEN` | 72 | P02 | P03 | Ambas en rama error no-SPI |
| D13 | prepare result `!= 0` | 275 | N92 | L00 | Ambas, determinada por D10/D14/D20 |
| D14 | presence status `!= OK` | 200 | D15 | P14 | Ambas |
| D15 | `snapshot == NULL` | 28 | P07 | P06 | T infeasible: caller pasa `&snapshot` |
| D16 | status `== NOT_DETECTED_ZERO` | 42-43 | P08 | D17 | Ambas en presencia fallida |
| D17 | status `== NOT_DETECTED_FLOATING` | 42,47 | P09 | D18 | Ambas si D16=F |
| D18 | status `== REGISTER_MISMATCH` | 42,51,61 | P10 | P12 | Ambas si D16/D17=F; F selecciona `default` |
| D19 | `snapshot != NULL` | 53 | P11 | P13 | F infeasible: caller pasa `&snapshot` |
| D20 | configure status `!= OK` | 208 | P15 | P16 | Ambas tras presencia OK |
| D21 | `csv_path == NULL` | 172 | L01 | L02 | Ambas |
| D22 | `fopen(...) == NULL` | 177 | L03 | L04 | Ambas si path no nulo |
| D23 | `live != 0` | 230 | D24 | D25 | T infeasible: raiz pasa literal 0 |
| D24 | `g_stop_requested == 0` | 230 | L05 | D25 | No evaluada desde raiz fija |
| D25 | `live == 0` | 231 | D26 | L10 | Siempre T desde raiz fija |
| D26 | `sample_index <= max_samples` | 231 | L05 | L10 | F natural infeasible antes del primer read y tras exitos: break interno termina |
| D27 | sample status `!= OK` | 236 | L06 | L07 | Ambas |
| D28 | `live == 0` | 246 | D29 | D30 | Siempre T desde raiz fija |
| D29 | `sample_index > max_samples` | 246 | L10 | D30 | Ambas: T en ultima muestra, F antes |
| D30 | `rate_hz == 0` | 158 | D23 | L08 | T infeasible despues de validacion OK |
| D31 | `nanosleep(...) != 0` | 164 | L09 | D23 | Ambas con timespec POSIX valido; para rate 1 y N>1, T persiste por `tv_nsec=1000000000` invalido |
| D32 | `csv != NULL` | 253 | L11 | L12 | Ambas segun D21/D22 |

Cada fila define sus dos edges salientes. Los nodos de accion y contratos tienen una unica salida en el DOT; sus labels incluyen la linea fuente. Por lo tanto el DOT actua como edge list completa y la tabla anterior como edge list de decisiones.

## Nodos de accion por flujo

| Grupo | Nodos | Fuente / significado |
|---|---|---|
| Root | N00-N01, N90-N92 | 260-283: entrada, log, close y returns |
| Salida sintetica | N99 | Join del modelo para returns de lineas 267, 276 y 282; no corresponde a una sentencia adicional |
| Validator | V00-V06 | 127-152: init errors, logs y returns |
| Preparation | P00-P16 | 26-82 y 187-217: contratos, diagnosticos, snapshot, close y returns |
| Loop/CSV/sleep | L00-L12 | 154-185 y 219-258: CSV, condicion, read, outputs, pacing, close y return |

Cada nodo ejecutable tiene referencia de fuente; N99 esta identificado como join sintetico. Los nodos verdes son contratos externos en el call site; no afirman que su implementacion interna este cubierta.

## Complejidad ciclomatica

Convencion: grafo conectado, decisiones atomicas binarias, helpers inlined en una sola raiz.

- Decisiones binarias `D01..D32`: 32.
- Complejidad: `V(G) = D + 1 = 32 + 1 = 33`.
- Interpretacion: 33 es la dimension estructural/basis-path upper bound del grafo normalizado, no la cantidad de paths factibles desde `acquisition_run_samples(..., live=0)`.

La cuenta por funcion/componente es:

| Region | Decisiones |
|---|---:|
| Root `acquisition_run_samples` | D01, D02, D13 = 3 |
| Validator | D03-D09 = 7 |
| Preparation + log/print helpers | D10-D12, D14-D20 = 10 |
| CSV + acquisition loop + sleep | D21-D32 = 12 |
| Total | 32 |

## Restricciones de factibilidad

1. Validacion exitosa implica `samples in [1,1000]`, `rate_hz in [1,10]`, `channel=0`.
2. La raiz pasa `live=0`; D23=T, D24 y D28=F no son alcanzables en este entry point.
3. El loop fijo no tiene path normal de cero iteraciones.
4. Tras la muestra N exitosa, D29=T ejecuta `break`; D26=F no termina el caso fijo normal.
5. `snapshot` es una variable local cuya direccion se pasa a ambos helpers; D15=T y D19=F son infeasibles.
6. `rate_hz=0` no alcanza D30 tras validacion exitosa.
7. Un canal invalido no alcanza `ads1263_configure_channel`.
8. Init failure suprime presencia/configuracion/loop. Presence failure suprime configuracion/loop. Configure failure suprime loop.
9. `fopen` failure no hace fallar acquisition en la fuente actual; continua con `csv=NULL`.
10. `nanosleep` puede repetir indefinidamente ante error persistente; solo se selecciona un retry seguido de exito.
11. Un mock que retorna `ADS1263_OK` para read debe inicializar todos los campos consumidos de `sample`; de lo contrario el test, no el producto, induce comportamiento indefinido.
12. Para `rate_hz=1`, la fuente construye `tv_nsec=1000000000`; POSIX lo rechaza con `EINVAL`. Por eso rate 1 solo se usa con N=1 (sin sleep) y los paths finitos con back edge usan rate 2. El comportamiento N>1/rate 1 queda como caracterizacion DSP-017, no como path finito exitoso.

## Paths estructurales no seleccionables desde esta raiz

| Path/edge | Razon |
|---|---|
| D15=T | `print_snapshot` recibe `&snapshot` |
| D19=F | `log_presence_error` recibe `&snapshot` |
| D23=T, D24=T/F | `acquire_loop` recibe `live=0` |
| D25=F | `live=0` |
| D26=F como terminacion fija normal | validacion exige N>=1 y la ultima iteracion rompe en D29 |
| D28=F | `live=0` |
| D30=T | validacion exige rate>=1 |
| D31=F con rate=1 y N>1 bajo `nanosleep` POSIX real | timespec invalido produce `EINVAL`; el retry no progresa sin intervencion no realista |
| Configure con `channel!=0` | validator retorna antes de preparation |

Estos edges permanecen en el CFG porque pertenecen a helpers compartidos, pero no se cuentan como casos factibles de la raiz fija. Live mode requiere un modelo separado de `acquisition_run_live()`.

## Seleccion de paths

Se seleccionan 24 paths factibles que cubren cada outcome top-level y helper alcanzable, cada diagnostico de init/presencia, CSV sin path/fallo/exito, loop de una y dos iteraciones, error temprano/tardio y un retry de sleep. Se agregan valores limite a esos paths; no se pretende ejecutar 1000 sleeps.

La secuencia, precondicion y oraculo de cada path se detallan en `docs/tsse/cft_cases.md`. El conjunto seleccionado cubre los edges factibles relevantes del modelo, pero no se declara “all paths”: existen combinaciones de errores acumulados, N iteraciones y retries ambientales potencialmente no acotadas.

## Comandos de generacion

Desde la raiz del repositorio:

```bash
dot -Tsvg docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.dot -o docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.svg
dot -Tpng docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.dot -o docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.png
dot -Tdot docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.dot -o /tmp/opencode/acquisition_run_samples_cfg.normalized.dot
```

El tercer comando valida parseo de Graphviz sin modificar evidencia del repositorio.

## Limites de la afirmacion

El modelo permite afirmar cobertura de diseno sobre control de acquisition dentro de la frontera declarada. No verifica paths internos del driver, HAL Linux, hardware, persistencia fisica de CSV, exactitud ADC, frecuencia real ni timing de target. Ningun caso se considera ejecutado en Milestone 1.
