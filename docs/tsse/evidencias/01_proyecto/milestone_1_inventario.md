# Inventario auditado - Milestone 1

## Alcance de la inspeccion

- Revision de produccion: `c59b24017f5daebb4e37512d799a0d07f589bce5`.
- Revision documental inspeccionada: `8dc23861e8bf87345f20218c587244329e1caf6f`.
- Metodo: lectura estatica de fuentes, cabeceras, `Makefile` y documentacion.
- Estado: diseno; no se ejecutaron casos de prueba.

## Modulos de produccion

| Modulo | Fuentes | Responsabilidad observada | Dependencias directas |
|---|---|---|---|
| CLI | `main/main.c` | Parseo de opciones, limites CLI y seleccion de modo | acquisition, ADS1263 defaults, logger, libc |
| Acquisition | `components/acquisition/src/acquisition.c`, `include/acquisition.h` | Check, validacion, preparacion ADC, adquisicion fija/live, CSV y pacing | ADS1263 API, logger, stdio, `signal`, `nanosleep` |
| ADS1263 core | `components/ads1263/src/ads1263.c`, `include/ads1263.h`, `include/ads1263_defs.h` | Configuracion, registros, presencia, canal, DRDY, muestra y conversion | HAL SPI/GPIO Linux, `nanosleep`, libc |
| SPI Linux | `components/ads1263/arch/linux/src/ads1263_spi_linux.c`, cabecera asociada | Apertura/configuracion/transferencia `spidev` | `open`, `ioctl`, `close`, Linux UAPI |
| GPIO Linux | `components/ads1263/arch/linux/src/ads1263_gpio_linux.c`, cabecera asociada | Solicitud y lectura DRDY | libgpiod v2 o rama `ADS1263_NO_GPIOD` |
| Logger | `components/logger/src/app_log.c`, `include/app_log.h` | INFO/WARN/ERROR, fila de consola y fila CSV | stdio variadico |

## Entradas de build de produccion

El `Makefile` enlaza `main`, logger, ADS1263 core, ambos adaptadores Linux y acquisition en `build/ads1263_app`. Usa C11, `_POSIX_C_SOURCE=200809L`, warnings, los cuatro directorios publicos de cabeceras y libgpiod salvo `NO_GPIOD=1`.

No se encontro `project.yml`, fuente de tests Unity/Ceedling, runner generado ni configuracion de cobertura en la revision inspeccionada. Su ausencia es estado inicial, no resultado de una ejecucion.

## Funciones publicas inventariadas

### Acquisition

| Funcion | Fuente | Rol de prueba previsto |
|---|---|---|
| `acquisition_default_config()` | `acquisition.c:13-24` | Valores predeterminados |
| `acquisition_run_check()` | `acquisition.c:84-125` | Flujo de presencia/DRDY |
| `acquisition_run_samples()` | `acquisition.c:260-283` | Raiz del modelo CFT formal |
| `acquisition_run_live()` | `acquisition.c:291-317` | Integracion host posterior, fuera de Milestone 1 |

### ADS1263 core

| Funcion | Fuente | Rol de prueba previsto |
|---|---|---|
| `ads1263_default_config()` | `ads1263.c:7-21` | Defaults del dispositivo |
| `ads1263_init()` / `ads1263_close()` | `ads1263.c:40-90` | Ciclo de vida y cleanup |
| `ads1263_read_registers()` | `ads1263.c:92-125` | SUT prioritario: limites y RREG |
| `ads1263_write_registers()` | `ads1263.c:127-154` | SUT prioritario: limites y WREG |
| `ads1263_check_presence()` | `ads1263.c:167-207` | SUT prioritario: clasificacion de registros |
| `ads1263_configure_channel()` | `ads1263.c:225-255` | SUT prioritario: INPMUX/readback/START1 |
| `ads1263_read_drdy()` | `ads1263.c:257-269` | Contrato DRDY |
| `ads1263_read_sample()` | `ads1263.c:313-353` | SUT prioritario: espera, RDATA1 y parseo |
| `ads1263_raw_to_voltage()` | `ads1263.c:355-361` | SUT prioritario: conversion y limites |
| `ads1263_status_to_string()` | `ads1263.c:363-395` | Mapeo estable de estados |

### Adaptadores Linux y logger

| Grupo | API publica | Limite de aislamiento futuro |
|---|---|---|
| SPI | `ads1263_spi_linux_open/transfer/close` | Mock para ADS1263 core; wrappers de syscalls solo en tests del adaptador |
| GPIO | `ads1263_gpio_linux_open/read_drdy/close` | Mock para ADS1263 core; fake libgpiod solo en tests del adaptador |
| Logger | `app_log_info/warn/error/sample/csv_sample` | Spy manual o captura de streams; no depender de comparacion CMock de varargs |

## Limites del modelo CFT

| Clasificacion | Elementos |
|---|---|
| Incluidos e interprocedurales | `acquisition_run_samples`, `validate_acquisition_config`, `prepare_adc_for_acquisition`, `log_init_error`, `print_snapshot`, `log_presence_error`, `acquire_loop`, `open_csv_if_requested`, `sleep_for_rate` |
| Contratos externos controlables | ADS1263 public API, logger, `fopen/fputs/fclose`, `nanosleep` |
| Excluidos del grafo | Implementacion interna ADS1263, adaptadores Linux, CLI, live/SIGINT, hardware fisico |

## Relacion SUT-dependencia

| SUT o flujo | Dependencia que debe controlarse | Observacion verificable futura |
|---|---|---|
| Registro ADS1263 | transferencia SPI | bytes TX, longitud, RX y estado |
| Presencia ADS1263 | lectura RREG interna via HAL | clasificacion 0x00/0xFF/mismatch/OK |
| Configuracion de canal | HAL SPI | orden WREG, RREG, readback y START1 |
| Lectura de muestra | GPIO, SPI, espera | polls DRDY, RDATA1, parseo y estado |
| Acquisition CFT | ADS1263 API | orden init/presencia/config/read/close |
| CSV acquisition | stdio | path nulo, apertura fallida/exito, header, filas y cierre |
| Pacing acquisition | `nanosleep` | cantidad de sleeps y retry controlado |

## Conclusion de auditoria

La arquitectura ofrece limites de enlace suficientes para planificar pruebas host sin modificar produccion. La cabecera `ads1263.h` expone tipos Linux, por lo que el aislamiento es de implementacion/directorio y no una abstraccion completa de plataforma; esta discrepancia se mantiene abierta en el registro documental.
