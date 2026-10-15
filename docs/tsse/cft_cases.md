# Casos derivados del CFT - `acquisition_run_samples()`

## Estado, alcance y convenciones

- Tipo: diseno de casos; ningun caso fue implementado ni ejecutado.
- Estado de todos los casos: `NOT RUN`.
- Modelo: `docs/tsse/cft_acquisition_run_samples.md`, baseline `c59b24017f5daebb4e37512d799a0d07f589bce5`.
- Frontera: componente acquisition con ADS1263, logger, archivo y tiempo sustituidos por contratos controlados.
- Afirmacion: cobertura de diseno del conjunto factible seleccionado; no es una afirmacion de todos los paths.

`BASE` significa: `ADC0={spi_device="/dev/fake-spi",gpio_chip="fakechip",drdy_line=8}` no nulo; `ACQ0={samples=1,rate_hz=2,channel=0,csv_path=NULL}` no nulo salvo override; doubles deterministas sin acceso a hardware; logger/event spy limpio; directorio temporal limpio. La accion `RUN` es invocar una vez `acquisition_run_samples(adc_config, acq_config)` y capturar retorno, eventos ordenados, llamadas y efectos de archivo.

Fixtures de outcomes observables:

- `SNAP_ZERO={00,00,00,00,00}`; `SNAP_FF={FF,FF,FF,FF,FF}`.
- `SNAP_BAD={id=01,power=11,interface=05,mode0=00,mode1=00}`; `SNAP_OK={id=01,power=11,interface=05,mode0=00,mode1=80}`.
- `S1={raw=100,voltage=0.125,drdy=0,status_byte=0x40,checksum_byte=0xA1,status="OK"}`.
- `S2={raw=-100,voltage=-0.125,drdy=0,status_byte=0x40,checksum_byte=0xA2,status="OK"}`.
- `CFG(N,R,C)` es el evento info de configuracion; `SNAP(x)` son las cinco lineas ID/POWER/INTERFACE/MODE0/MODE1; `OUT(i,S)` son, en orden, `app_log_sample(i,&S)` y `app_log_csv_sample(csv,i,&S)`.

En las cuentas se usa `I/P/C/R/S/X/T` para `init/presence/configure/read/nanosleep/ads1263_close/status_to_string`. `O` indica pares de llamadas de salida `app_log_sample/app_log_csv_sample`. `F=open/header/close` cuenta `fopen/fputs/fclose`. Una cuenta cero tambien es una expectativa que debe verificarse.

## Criterio de seleccion

Se seleccionan 24 paths estructurales factibles orientados a edges y outcomes. P01-P16 introducen rechazos, short-circuit o outcomes de preparacion distintos; P17-P23 cubren las tres ramas CSV, exito fijo, errores de primera/segunda lectura, back edge y retry de sleep; P24 agrega el limite superior aceptado con CSV abierto y fallo temprano, sin ejecutar 1000 esperas. Debido a edges infeasibles desde la raiz fija y retries ambientales no acotados, este conjunto no es una base completa de dimension 33 ni cobertura exhaustiva de paths.

## Paths seleccionados

Para mantener las secuencias auditables sin repetir cientos de nodos, se definen prefijos exactos y contiguos:

- `VALID_OK = N00-D01F-V00-D03F-D04F-D05F-D06F-D07F-D08F-D09T-V05-D02F-N01`.
- `PREP_OK = P00-D10F-P05-D14F-P14-D20F-P16-D13F`.
- `NOCSV_ENTRY = L00-D21T-L01-D23F-D25T-D26T`.
- `CSVFAIL_ENTRY = L00-D21F-L02-D22T-L03-D23F-D25T-D26T`.
- `CSVOK_ENTRY = L00-D21F-L02-D22F-L04-D23F-D25T-D26T`.

El operador `+` concatena esos prefijos con la secuencia escrita; no representa nodos omitidos o indefinidos.

| ID | Secuencia de nodos | Predicado/outcome distintivo | Factibilidad |
|---|---|---|---|
| CFT-P01 | N00-D01T-N92-N99 | `adc_config=NULL`; short-circuit antes del validator | El puntero nulo habilita D01=T y retorna sin dependencias |
| CFT-P02 | N00-D01F-V00-D03T-V01-D02T-N92-N99 | `acq_config=NULL`; validator retorna -1 | El puntero de acquisition nulo habilita D03=T despues de D01=F |
| CFT-P03 | N00-D01F-V00-D03F-D04T-V02-D06F-D07F-D08F-D09F-V06-D02T-N92-N99 | `samples=0` | Cero es el unico unsigned que satisface D04; los demas campos son validos |
| CFT-P04 | N00-D01F-V00-D03F-D04F-D05T-V02-D06F-D07F-D08F-D09F-V06-D02T-N92-N99 | `samples=1001` | El valor supera el limite y habilita D05=T |
| CFT-P05 | N00-D01F-V00-D03F-D04F-D05F-D06T-V03-D08F-D09F-V06-D02T-N92-N99 | `rate=0` | Cero habilita D06=T; samples y channel son validos |
| CFT-P06 | N00-D01F-V00-D03F-D04F-D05F-D06F-D07T-V03-D08F-D09F-V06-D02T-N92-N99 | `rate=11` | El valor supera el limite y habilita D07=T |
| CFT-P07 | N00-D01F-V00-D03F-D04F-D05F-D06F-D07F-D08T-V04-D09F-V06-D02T-N92-N99 | `channel=1` | La clase no soportada habilita D08=T y suprime configure |
| CFT-P08 | N00-D01F-V00-D03F-D04T-V02-D06F-D07T-V03-D08T-V04-D09F-V06-D02T-N92-N99 | tres campos invalidos acumulados | Los tres valores fuerzan tres logs; D05 se omite por short-circuit de D04 |
| CFT-P09 | VALID_OK + P00-D10T-D11T-P01-P04-D13T-N92-N99 | init=`SPI_OPEN` | El double puede retornar SPI_OPEN; fuente retorna sin close |
| CFT-P10 | VALID_OK + P00-D10T-D11F-D12T-P02-P04-D13T-N92-N99 | init=`GPIO_OPEN` | El double puede retornar GPIO_OPEN despues de D11=F |
| CFT-P11 | VALID_OK + P00-D10T-D11F-D12F-P03-P04-D13T-N92-N99 | init=`SPI_CONFIG` | Outcome externo alcanzable selecciona diagnostico generico |
| CFT-P12 | VALID_OK + P00-D10F-P05-D14T-D15F-P06-P07-D16T-P08-P13-D13T-N92-N99 | presence=`ZERO` | Outcome inyectado y `&snapshot` no nulo habilitan D15=F/D16=T |
| CFT-P13 | VALID_OK + P00-D10F-P05-D14T-D15F-P06-P07-D16F-D17T-P09-P13-D13T-N92-N99 | presence=`FLOATING` | Outcome inyectado habilita D17=T despues de D16=F |
| CFT-P14 | VALID_OK + P00-D10F-P05-D14T-D15F-P06-P07-D16F-D17F-D18T-P10-D19T-P11-P13-D13T-N92-N99 | presence=`MISMATCH` | Outcome inyectado y `&snapshot` habilitan D18=T/D19=T |
| CFT-P15 | VALID_OK + P00-D10F-P05-D14T-D15F-P06-P07-D16F-D17F-D18F-P12-P13-D13T-N92-N99 | presence=`IO` | Otro error inyectado alcanza default y texto de status |
| CFT-P16 | VALID_OK + P00-D10F-P05-D14F-P14-D20T-P15-D13T-N92-N99 | configure=`SPI_TRANSFER` | Init/presence OK hacen alcanzable configure; su error suprime loop |
| CFT-P17 | VALID_OK + PREP_OK + NOCSV_ENTRY + L05-D27F-L07-D28T-D29T-L10-D32F-L12-N90-N91-N99 | sin CSV, N=1 exitoso | N=1 hace D29=T tras la primera muestra y evita sleep |
| CFT-P18 | VALID_OK + PREP_OK + CSVFAIL_ENTRY + L05-D27F-L07-D28T-D29T-L10-D32F-L12-N90-N91-N99 | CSV solicitado, open falla, N=1 exitoso | `fopen=NULL` retorna csv nulo y la fuente continua |
| CFT-P19 | VALID_OK + PREP_OK + CSVOK_ENTRY + L05-D27F-L07-D28T-D29T-L10-D32T-L11-L12-N90-N91-N99 | CSV abre, header, N=1, cierre | Stream valido habilita header y D32=T |
| CFT-P20 | VALID_OK + PREP_OK + NOCSV_ENTRY + L05-D27T-L06-L10-D32F-L12-N90-N91-N99 | primera lectura=`SPI_TRANSFER` | Error inyectado habilita break antes de output/sleep |
| CFT-P21 | VALID_OK + PREP_OK + NOCSV_ENTRY + L05-D27F-L07-D28T-D29F-D30F-L08-L09-D31F-D23F-D25T-D26T-L05-D27F-L07-D28T-D29T-L10-D32F-L12-N90-N91-N99 | N=2 exitoso, un back edge | Rate 2 produce timespec valido; sleep OK permite segunda iteracion |
| CFT-P22 | VALID_OK + PREP_OK + NOCSV_ENTRY + L05-D27F-L07-D28T-D29F-D30F-L08-L09-D31F-D23F-D25T-D26T-L05-D27T-L06-L10-D32F-L12-N90-N91-N99 | segunda lectura=`SPI_TRANSFER` | Primera lectura y sleep validos alcanzan el segundo error |
| CFT-P23 | VALID_OK + PREP_OK + NOCSV_ENTRY + L05-D27F-L07-D28T-D29F-D30F-L08-L09-D31T-L09-D31F-D23F-D25T-D26T-L05-D27F-L07-D28T-D29T-L10-D32F-L12-N90-N91-N99 | un `nanosleep` interrumpido y luego OK | Rate 2 da timespec valido; `EINTR` con remainder controlado habilita un retry finito |
| CFT-P24 | VALID_OK + PREP_OK + CSVOK_ENTRY + L05-D27T-L06-L10-D32T-L11-L12-N90-N91-N99 | N=1000, rate=10, CSV abierto, primera lectura falla | Limites validos alcanzan loop; CSV hace el path distinto y el error evita 1000 esperas |

## Registros atomicos y oraculos

Los enlaces REQ en esta tabla califican solo la respuesta de orchestration observable. Cuando el outcome ADS1263 se inyecta, el caso no verifica el protocolo interno, el bus, GPIO ni hardware; TST-003 es el requisito directo del diseno CFT.

| ID | Fuente / alcance | Precondiciones e inputs | Script de dependencias | Retorno, logs y outputs esperados | Cuentas esperadas | Archivo y cleanup | Estado / evidencia / defecto |
|---|---|---|---|---|---|---|---|
| CFT-P01 | REQ-011, TST-003 | BASE; `adc_config=NULL`; acq `{N=1,rate=1,ch=0,csv=NULL}` | Ninguna llamada permitida | `-1`; sin logs ni muestras | `I/P/C/R/S/X/T=0/0/0/0/0/0/0`; `O=0` | `F=0/0/0`; sin cleanup | `NOT RUN`; `evidencias/04_unitarias/CFT-P01/`; defecto N/A |
| CFT-P02 | REQ-011, TST-003 | BASE; `acq_config=NULL` | Ninguna ADS1263 permitida | `-1`; error `invalid acquisition configuration`; sin muestras | `0/0/0/0/0/0/0`; `O=0` | `F=0/0/0`; sin cleanup | `NOT RUN`; `evidencias/04_unitarias/CFT-P02/`; defecto N/A |
| CFT-P03 | REQ-011, TST-003 | BASE; `{N=0,rate=1,ch=0,csv=NULL}` | Ninguna ADS1263 permitida | `-1`; error de rango samples; sin muestras | `0/0/0/0/0/0/0`; `O=0` | `F=0/0/0`; sin cleanup | `NOT RUN`; `evidencias/04_unitarias/CFT-P03/`; defecto N/A |
| CFT-P04 | REQ-011, TST-003 | BASE; `{N=1001,rate=1,ch=0,csv=NULL}` | Ninguna ADS1263 permitida | `-1`; error de rango samples; sin muestras | `0/0/0/0/0/0/0`; `O=0` | `F=0/0/0`; sin cleanup | `NOT RUN`; `evidencias/04_unitarias/CFT-P04/`; defecto N/A |
| CFT-P05 | REQ-011, TST-003 | BASE; `{N=1,rate=0,ch=0,csv=NULL}` | Ninguna ADS1263 permitida | `-1`; error de rango rate; sin muestras | `0/0/0/0/0/0/0`; `O=0` | `F=0/0/0`; sin cleanup | `NOT RUN`; `evidencias/04_unitarias/CFT-P05/`; defecto N/A |
| CFT-P06 | REQ-011, TST-003 | BASE; `{N=1,rate=11,ch=0,csv=NULL}` | Ninguna ADS1263 permitida | `-1`; error de rango rate; sin muestras | `0/0/0/0/0/0/0`; `O=0` | `F=0/0/0`; sin cleanup | `NOT RUN`; `evidencias/04_unitarias/CFT-P06/`; defecto N/A |
| CFT-P07 | REQ-007, REQ-011, TST-003 | BASE; `{N=1,rate=1,ch=1,csv=NULL}` | Ninguna ADS1263 permitida | `-1`; error de rango channel; sin muestras | `0/0/0/0/0/0/0`; `O=0` | `F=0/0/0`; sin cleanup | `NOT RUN`; `evidencias/04_unitarias/CFT-P07/`; defecto N/A |
| CFT-P08 | REQ-011, TST-003 | BASE; `{N=0,rate=11,ch=1,csv=NULL}` | Ninguna ADS1263 permitida | `-1`; tres errores en orden samples/rate/channel | `0/0/0/0/0/0/0`; `O=0` | `F=0/0/0`; sin cleanup | `NOT RUN`; `evidencias/04_unitarias/CFT-P08/`; defecto N/A |
| CFT-P09 | TST-003 | BASE | init=`SPI_OPEN`; prohibir llamadas posteriores | `-1`; `CFG(1,2,0)`; errores `failed to open SPI device: /dev/fake-spi` y guidance | `1/0/0/0/0/0/0`; `O=0` | `F=0/0/0`; fuente no cierra init fallido | `NOT RUN`; `evidencias/04_unitarias/CFT-P09/`; defecto N/A |
| CFT-P10 | TST-003 | BASE | init=`GPIO_OPEN`; prohibir llamadas posteriores | `-1`; `CFG(1,2,0)`; error con `chip=fakechip line=8` y guidance | `1/0/0/0/0/0/0`; `O=0` | `F=0/0/0`; fuente no cierra init fallido | `NOT RUN`; `evidencias/04_unitarias/CFT-P10/`; defecto N/A |
| CFT-P11 | TST-003 | BASE | init=`SPI_CONFIG`; status text=`SPI_CONFIG` | `-1`; `CFG(1,2,0)`; error `failed to initialize ADS1263: SPI_CONFIG` | `1/0/0/0/0/0/1`; `O=0` | `F=0/0/0`; fuente no cierra init fallido | `NOT RUN`; `evidencias/04_unitarias/CFT-P11/`; defecto N/A |
| CFT-P12 | TST-003; contexto REQ-005 parcial | BASE | init=OK; presence=ZERO + `SNAP_ZERO` | `-1`; `CFG(1,2,0)`; `SNAP(SNAP_ZERO)`; errores all-zero y guidance | `1/1/0/0/0/1/0`; `O=0` | `F=0/0/0`; close tras presence | `NOT RUN`; `evidencias/04_unitarias/CFT-P12/`; defecto N/A |
| CFT-P13 | TST-003; contexto REQ-005 parcial | BASE | init=OK; presence=FLOATING + `SNAP_FF` | `-1`; `CFG(1,2,0)`; `SNAP(SNAP_FF)`; errores all-FF y guidance | `1/1/0/0/0/1/0`; `O=0` | `F=0/0/0`; close tras presence | `NOT RUN`; `evidencias/04_unitarias/CFT-P13/`; defecto N/A |
| CFT-P14 | TST-003; contexto REQ-004 parcial | BASE | init=OK; presence=MISMATCH + `SNAP_BAD` | `-1`; `CFG(1,2,0)`; `SNAP(SNAP_BAD)`; mismatch y baseline esperado | `1/1/0/0/0/1/0`; `O=0` | `F=0/0/0`; close tras presence | `NOT RUN`; `evidencias/04_unitarias/CFT-P14/`; defecto N/A |
| CFT-P15 | TST-003; contexto REQ-004 parcial | BASE | init=OK; presence=`IO` + `SNAP_OK`; status text=`IO` | `-1`; `CFG(1,2,0)`; `SNAP(SNAP_OK)`; error `ADS1263 check failed: IO` | `1/1/0/0/0/1/1`; `O=0` | `F=0/0/0`; close tras presence | `NOT RUN`; `evidencias/04_unitarias/CFT-P15/`; defecto N/A |
| CFT-P16 | REQ-007 parcial, TST-003 | BASE | init=OK; presence=OK; configure=`SPI_TRANSFER`; status text igual | `-1`; `CFG(1,2,0)`; error `failed to configure ADS1263 channel 0: SPI_TRANSFER`; sin reads | `1/1/1/0/0/1/1`; `O=0` | `F=0/0/0`; close tras configure | `NOT RUN`; `evidencias/04_unitarias/CFT-P16/`; defecto N/A |
| CFT-P17 | REQ-007 parcial, REQ-008, TST-003 | BASE; `{N=1,rate=1,ch=0,csv=NULL}` | preparation=OK; read1=OK + `S1` | `0`; `CFG(1,1,0)`; `OUT(1,S1)` | `1/1/1/1/0/1/0`; `O=1` | `F=0/0/0`; ADC close final; ningun sleep | `NOT RUN`; `evidencias/04_unitarias/CFT-P17/`; defecto N/A |
| CFT-P18 | REQ-008, REQ-010 caracterizacion, TST-003 | BASE; `{N=1,rate=1,ch=0,csv=unavailable}` | preparation=OK; fopen=NULL; read1=OK + `S1` | `0`; `CFG(1,1,0)`; warnings `could not open`/`continuing`; `OUT(1,S1)` con csv NULL | `1/1/1/1/0/1/0`; `O=1` | `F=1/0/0`; ADC close; DSP-006 | `NOT RUN`; `evidencias/04_unitarias/CFT-P18/`; defecto N/A |
| CFT-P19 | REQ-008, REQ-010, TST-003 | BASE; `{N=1,rate=1,ch=0,csv=temp}` | preparation=OK; fopen=stream; read1=OK + `S1` | `0`; `CFG(1,1,0)`; `OUT(1,S1)` | `1/1/1/1/0/1/0`; `O=1` | `F=1/1/1`; crear/truncar, header exacto, llamada logger con stream, cierre CSV antes de ADC; formato de fila es contrato externo | `NOT RUN`; `evidencias/04_unitarias/CFT-P19/`; defecto N/A |
| CFT-P20 | REQ-008, ROB-002, TST-003 | BASE; `{N=1,rate=1,ch=0,csv=NULL}` | preparation=OK; read1=`SPI_TRANSFER`; status text igual | `-1`; `CFG(1,1,0)`; error `sample read failed: SPI_TRANSFER`; sin `OUT` | `1/1/1/1/0/1/1`; `O=0` | `F=0/0/0`; ADC close final | `NOT RUN`; `evidencias/04_unitarias/CFT-P20/`; defecto N/A |
| CFT-P21 | REQ-007 parcial, REQ-008, TST-003 | BASE; `{N=2,rate=2,ch=0,csv=NULL}` | preparation=OK; read1=OK+S1; sleep=0; read2=OK+S2 | `0`; `CFG(2,2,0)`; `OUT(1,S1)`; `OUT(2,S2)` | `1/1/1/2/1/1/0`; `O=2` | `F=0/0/0`; un back edge, un sleep, ADC close | `NOT RUN`; `evidencias/04_unitarias/CFT-P21/`; defecto N/A |
| CFT-P22 | REQ-008, ROB-002, TST-003 | BASE; `{N=2,rate=2,ch=0,csv=NULL}` | preparation=OK; read1=OK+S1; sleep=0; read2=`SPI_TRANSFER`; status text=`SPI_TRANSFER` | `-1`; `CFG(2,2,0)`; `OUT(1,S1)`; error `sample read failed: SPI_TRANSFER`; sin `OUT(2)` | `1/1/1/2/1/1/1`; `O=1` | `F=0/0/0`; ADC close final | `NOT RUN`; `evidencias/04_unitarias/CFT-P22/`; defecto N/A |
| CFT-P23 | REQ-008, TST-003 | BASE; `{N=2,rate=2,ch=0,csv=NULL}` | preparation=OK; reads=S1/S2; nanosleep1=`-1/EINTR` y remainder `{0,250000000}`; nanosleep2=0 | `0`; `CFG(2,2,0)`; `OUT(1,S1)`; `OUT(2,S2)`; retry no agrega output | `1/1/1/2/2/1/0`; `O=2` | `F=0/0/0`; un sleep logico/dos calls; ADC close | `NOT RUN`; `evidencias/04_unitarias/CFT-P23/`; defecto N/A |
| CFT-P24 | REQ-008, REQ-010 parcial, ROB-002, TST-003 | BASE; `{N=1000,rate=10,ch=0,csv=temp}` | preparation=OK; fopen=stream; read1=`SPI_TRANSFER`; status text igual | `-1`; `CFG(1000,10,0)`; error `sample read failed: SPI_TRANSFER`; sin `OUT` | `1/1/1/1/0/1/1`; `O=0` | `F=1/1/1`; crear/truncar y header sin filas; cierre CSV antes de ADC; sin 1000 waits | `NOT RUN`; `evidencias/04_unitarias/CFT-P24/`; defecto N/A |

Todos los registros usan accion `RUN`. Las rutas de evidencia escritas como `evidencias/...` son relativas a `docs/tsse/`; son destinos previstos y no afirman que esos directorios o resultados existan aun. `Defecto N/A` significa que el caso no esta vinculado a un defecto confirmado; ROB-002 permanece hipotesis de robustez. P19/P24 verifican lifecycle/header y la llamada al contrato logger, no expanden ni atribuyen a acquisition el formato interno de la fila.

## Matriz explicita de limites y equivalencias

| Campo | Valor | Particion | Caso | Resultado esperado de diseno |
|---|---:|---|---|---|
| samples | 0 | inferior invalido | CFT-P03 | validator retorna -1; init no llamado |
| samples | 1 | inferior valido | CFT-P17 | una lectura y salida; cero sleeps; retorno 0 |
| samples | 1000 | superior valido | CFT-P24 | alcanza CSV/preparation/loop; fallo inyectado en primera lectura; retorno -1 |
| samples | 1001 | superior invalido | CFT-P04 | validator retorna -1; init no llamado |
| rate_hz | 0 | inferior invalido | CFT-P05 | validator retorna -1; `sleep_for_rate(0)` inalcanzable |
| rate_hz | 1 | inferior valido | CFT-P17 | alcanza loop; N=1 no duerme; N>1 queda en DSP-017 |
| rate_hz | 10 | superior valido | CFT-P24 | alcanza loop; fallo temprano evita esperas masivas |
| rate_hz | 11 | superior invalido | CFT-P06 | validator retorna -1; init no llamado |
| channel | 0 | unica clase soportada | CFT-P17 | alcanza `ads1263_configure_channel(0)` |
| channel | 1 | clase no soportada | CFT-P07 | validator retorna -1; configure no llamado |

## Cobertura del conjunto y exclusiones justificadas

| Objetivo | Casos |
|---|---|
| Rechazo y short-circuit de validacion | CFT-P01..P08 |
| Init SPI/GPIO/otro | CFT-P09..P11 |
| Presence ZERO/FLOATING/MISMATCH/otro | CFT-P12..P15 |
| Configure failure | CFT-P16 |
| Sin CSV / open failure / CSV exitoso | CFT-P17..P19 |
| Primera y segunda lectura fallida | CFT-P20, CFT-P22 |
| N=1 sin sleep / N=2 con back edge | CFT-P17, CFT-P21 |
| Retry de sleep | CFT-P23 |
| Limite N=1000 sin 1000 esperas | CFT-P24 |
| Cleanup presence/configure/loop y CSV | CFT-P12..P24 segun recurso adquirido |

No se seleccionan D15=T ni D19=F porque los callers pasan `&snapshot`; D23=T/D24, D25=F y D28=F pertenecen a live mode; D30=T contradice rate validado; D26=F no es terminacion normal fija; retries persistentes de `nanosleep` son ambientalmente no acotados. La combinacion N>1/rate 1 no se usa como path finito: el timespec POSIX invalido mantiene D31=T y se conserva como DSP-017. Esas exclusiones no se convierten en `PASS`: no son casos ejecutados.

## Revision de afirmaciones

- Cada path seleccionado tiene exactamente un caso CFT-P01..P24 y estado `NOT RUN`.
- Los oraculos se limitan a retorno, eventos, conteos y efectos de archivo observables en host.
- El conjunto no verifica internals ADS1263, protocolo HAL, SPI/GPIO fisico, ADC, hardware, frecuencia real, timing de target ni persistencia fisica del filesystem.
- No se afirma cobertura exhaustiva de combinaciones de errores acumulados, iteraciones o retries.
- El fallo de apertura CSV se trata como caracterizacion de fuente bajo DSP-006, no como conformidad normativa ni defecto.
