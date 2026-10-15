# Evidencia de linea base - Milestone 1

## Clasificacion

- Actividad: auditoria documental y preservacion de linea base.
- Tipo: evidencia de inspeccion; no es ejecucion de pruebas.
- Dependencias: repositorio Git local; no se utilizo hardware, SDK de target, SPI ni GPIO.
- Fecha UTC: `2026-10-15T10:08:44+00:00`.
- Directorio de trabajo: raiz del repositorio `IdS_project`.

## Revisiones

| Campo | Valor |
|---|---|
| Rama | `feature/tsse-testing` |
| Commit original de produccion | `c59b24017f5daebb4e37512d799a0d07f589bce5` |
| HEAD al iniciar Milestone 1 | `8dc23861e8bf87345f20218c587244329e1caf6f` |
| Relacion observada | HEAD agrega documentos, OpenSpec y herramientas; la guarda de produccion no encontro diferencias en C/H ni Makefile respecto del commit original |
| Estado inicial del worktree | limpio (`git status --short --branch --untracked-files=all` mostro solo `## feature/tsse-testing`) |

El commit HEAD no reemplaza a la linea base del producto. La referencia para comprobar que el software de produccion permanece intacto es `c59b24017f5daebb4e37512d799a0d07f589bce5`.

## Entorno observado

| Elemento | Valor |
|---|---|
| Distribucion | Ubuntu 26.04.1 LTS |
| Kernel / arquitectura | Linux 7.0.0-38-generic x86_64 |
| Git | 2.53.0 |
| GCC disponible | 15.2.0 |
| OpenSpec CLI | 1.14.2 |
| Graphviz `dot` | 14.1.2 |

La presencia de una herramienta no demuestra que una actividad posterior haya sido ejecutada.

## Checksums de produccion

Algoritmo: SHA-256.

| Archivo | SHA-256 |
|---|---|
| `main/main.c` | `21888880c50117c41dff067713c194f80c097d99ac9a607403d21b6ee13468b1` |
| `components/logger/src/app_log.c` | `e981b8fcaff2762888148940d95b580a518d25d057c5a4562c36c03f582f2587` |
| `components/logger/include/app_log.h` | `acc8db7489c950f364644d0d50ac47bf59a8a9f28474607be92f57b2512511bd` |
| `components/acquisition/src/acquisition.c` | `0c37acc3f8283c507a9cd981878e3891a428b9e6fdb9f5457033dd0e2418ad06` |
| `components/acquisition/include/acquisition.h` | `00f11bb2cd1534df9ef8354de3408f04648e0071c4078bae69cab861b658ba59` |
| `components/ads1263/src/ads1263.c` | `01e6e0b888886e46669c72a2bfc5d8aa40bb1571bcc038e784178048de01fd0c` |
| `components/ads1263/include/ads1263.h` | `9e3fa167904644180238e671e44d4fa2d331d2bbe30b2804ee75738bf62baa6a` |
| `components/ads1263/include/ads1263_defs.h` | `102b383e280c12771a02a0d5b6ef02f4a254553450a14ce76e92bfc64f866001` |
| `components/ads1263/arch/linux/src/ads1263_spi_linux.c` | `af2d71df33bf33be94d58c5df4a01ab077da00eb1553276dbbc9c242c6766b94` |
| `components/ads1263/arch/linux/src/ads1263_gpio_linux.c` | `b94b5494559c60a9b081560f313d588da6891ffa908c3e49d1adc20761470d19` |
| `components/ads1263/arch/linux/include/ads1263_spi_linux.h` | `42b4213030a598d1095244fc5843e7f32b48d821374c809ad79dd708e679a7b2` |
| `components/ads1263/arch/linux/include/ads1263_gpio_linux.h` | `1a462dd502ed527a33e7c6b322677b914235728e4d8f3a8d3d918ca79734b584` |
| `Makefile` | `4bed2be2944bcb81fc551df6d4e40176c9720846e53166f1b6784d6a942f743c` |

## Comandos exactos

```bash
date --utc --iso-8601=seconds
lsb_release -ds
uname -srm
git branch --show-current
git rev-parse HEAD
git status --short --branch --untracked-files=all
git log --oneline -5
git diff --name-status c59b24017f5daebb4e37512d799a0d07f589bce5..HEAD
git --version
gcc --version
openspec --version
dot -V
sha256sum Makefile main/main.c components/logger/src/app_log.c components/logger/include/app_log.h components/acquisition/src/acquisition.c components/acquisition/include/acquisition.h components/ads1263/src/ads1263.c components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h
```

## Limitaciones

- Esta evidencia registra archivos y entorno; no valida comportamiento funcional.
- La validacion historica con STM32MP157D-DK1 y ADS1263 se conserva como contexto aportado por el proyecto, no como resultado de Milestone 1.
- No se ejecutaron pruebas, cobertura, analisis estatico ni sanitizers en esta actividad.
