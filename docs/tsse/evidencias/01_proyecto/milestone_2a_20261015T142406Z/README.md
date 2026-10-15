# Entorno y procedencia - Milestone 2A

## Identificacion

- Run set: `milestone_2a_20261015T142406Z`.
- Repositorio: `IdS_project`.
- Directorio de trabajo: `/home/cronos/projects/tsse/IdS_project`.
- Rama: `feature/tsse-testing`.
- HEAD durante las ejecuciones: `03cc58a1cb71183814a844c691b0c84da5f0bcae`.
- Baseline de produccion protegido: `c59b24017f5daebb4e37512d799a0d07f589bce5`.
- Estado del worktree: cambios TSSE no versionados en `.gitignore`, `project.yml`, `test/` y este arbol de evidencia; ningun cambio en C/H de produccion ni en `Makefile`.
- Identidad de inputs: `input_checksums.typescript` registra SHA-256 de la unidad de produccion, headers, configuraciones, soporte y tests usados.

La revision de tests no tiene commit propio durante esta ejecucion. Por eso se identifica con HEAD, estado del worktree y checksums, sin presentarla como una revision versionada.

## Host y herramientas

| Elemento | Valor observado | Evidencia raw |
|---|---|---|
| OS | Ubuntu 26.04.1 LTS | `os_release.typescript` |
| Kernel/arquitectura | Linux 7.0.0-38-generic, x86_64 | `uname.typescript` |
| GCC | 15.2.0 | `gcc_version.typescript` |
| Ceedling | 1.1.10-85ec551 | `ceedling_version.typescript` |
| Unity | 2.7.2, distribuido con Ceedling | `ceedling_version.typescript` |
| CMock | 2.7.2, distribuido con Ceedling | `ceedling_version.typescript` |
| gcov | 15.2.0, JSON format 2 | `gcov_version.typescript` |
| gcovr | 7.2 | `gcovr_version.typescript` |
| Ruby | 3.3.8 x86_64-linux-gnu | `ruby_version.typescript` |

`ruby_gems.typescript` confirma el gem local Ceedling 1.1.10. Unity y CMock no son gems locales separados: sus versiones son las informadas por `ceedling version`. No se instalaron ni sustituyeron paquetes.

## Configuracion inspeccionada

- `processed_project.yml`: configuracion normal resuelta.
- `processed_coverage.yml`: perfil coverage resuelto.
- `processed_sanitizer.yml`: perfil sanitizer resuelto; configurado pero no ejecutado en este milestone.
- `ceedling_source_inventory.typescript`: unico source declarado, `components/ads1263/src/ads1263.c`.
- `ceedling_include_inventory.typescript`: includes de ADS1263 core y adapters Linux; no hay SDK target.
- Salidas separadas: `build/ceedling/normal`, `build/ceedling/coverage` y `build/ceedling/sanitizer`.
- Lenguaje/feature defines: C11, `_POSIX_C_SOURCE=200809L` y soporte double explicito de Unity.

## Limites de dependencias

| Dependencia | Clasificacion en el run |
|---|---|
| `components/ads1263/src/ads1263.c` | Real, compilada directamente |
| SPI Linux | Mock generado por CMock desde `ads1263_spi_linux.h` |
| GPIO Linux | Mock generado por CMock desde `ads1263_gpio_linux.h` |
| `nanosleep` | Interpuesto por linker con `test/ceedling/support/wait_wrapper.c` |
| Unity/CMock/runtime C | Real, host |
| `/dev/spidev*`, `/dev/gpiochip*`, ADS1263 fisico | No usados |

El transcript de build muestra `ads1263.c`, ambos mocks generados y `wait_wrapper.c`; no se compilaron objetos de `components/ads1263/arch/linux/src/`.

## Guarda de produccion

Comandos ejecutados contra el baseline:

```sh
git diff --exit-code c59b24017f5daebb4e37512d799a0d07f589bce5 -- components/**/*.c components/**/*.h main/*.c Makefile
git diff --name-only c59b24017f5daebb4e37512d799a0d07f589bce5 -- components/**/*.c components/**/*.h main/*.c Makefile
```

Ambos terminaron con exit `0` y sin nombres de archivo. Los transcripts son `production_guard_diff.typescript` y `production_guard_names.typescript`. No se agregaron copias de algoritmos, ramas `UNIT_TEST`, exposicion de helpers static, cambios C/H de produccion ni cambios al `Makefile`.
