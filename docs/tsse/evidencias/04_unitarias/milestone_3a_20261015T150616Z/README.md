# Unit tests y regresion - Milestone 3A

## Baseline independiente

Antes de editar tests se ejecuto `ceedling clobber` y `ceedling test:all` con `project.yml`. Resultado: 62 tests, 62 PASS, 0 FAIL, 0 IGNORED, exit 0. Evidencia: `baseline/01_clobber.typescript`, `baseline/02_test_all.typescript`, JSON y JUnit. El inventario y resultado reproducen Milestone 2B de forma independiente.

## Resultado final

| Proyecto | Inventario unico | Runs limpios | Resultado por run | Dependencias |
|---|---:|---:|---|---|
| `project.yml` ADS1263 | 66 | 2 | 66 PASS, 0 FAIL, 0 IGNORED, exit 0 | `ads1263.c` real; SPI/GPIO CMock; waits wrapper |
| `project_acquisition_robustness.yml` | 6 | 2 | 6 PASS, 0 FAIL, 0 IGNORED, exit 0 | `acquisition.c` real; ADS1263 CMock; logger spy; waits wrapper/host POSIX controlado |

Inventario unitario unico total: 72 casos, 72 PASS, 0 FAIL, 0 IGNORED, 0 BLOCKED. Los dos runs ADS1263 estan en `final/01_*` a `04_*`; los dos runs acquisition finales estan en `final/07_*` a `10_*`, con JSON/JUnit separados.

El caso de proceso PROC-DSP017-001 no es un Unity PASS: su oracle de terminacion falla y el timeout externo retorna 124. Se reporta por separado en `../../07_defectos_regresion/milestone_3a_20261015T150616Z/` y no se oculta dentro del resumen Ceedling.

## Casos nuevos

- UT-REG-007/008: crossing-range mock read/write con efectos exactos y canarios.
- UT-DRDY-006/007: GPIO error/timeout seguidos por SPI_TRANSFER, muestra zeroed y espera acotada.
- UT-ACQ-ROB003-001..004: consecuencias acquisition para SPI success/failure tras estados DRDY.
- UT-ACQ-DSP017-001: captura tres retries bounded con timespec invalido.
- UT-ACQ-DSP017-002: rechazo EINVAL por `nanosleep` POSIX host.

Los casos ROB-002 y ROB-004 existentes se reprodujeron dentro de los runs ADS1263. No se debilito ni elimino ningun test.
