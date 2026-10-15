# Entorno y procedencia - Milestone 2B

## Identificacion

- Run set: `milestone_2b_20261015T144924Z`.
- Repositorio: `IdS_project`.
- Directorio de trabajo: `/home/cronos/projects/tsse/IdS_project`.
- Rama: `feature/tsse-testing`.
- HEAD durante las ejecuciones: `03cc58a1cb71183814a844c691b0c84da5f0bcae`.
- Baseline de produccion protegido: `c59b24017f5daebb4e37512d799a0d07f589bce5`.
- Estado del worktree: cambios TSSE no versionados o modificados en configuracion, tests, OpenSpec, trazabilidad y evidencia; ningun cambio en C/H de produccion ni en `Makefile`.
- Identidad de inputs: `input_checksums.typescript` registra SHA-256 de la unidad de produccion, headers, configuraciones, soporte y cinco suites usadas.

La revision de tests no tiene commit propio durante esta ejecucion. Por eso se identifica con HEAD, estado del worktree y checksums, sin presentarla como una revision versionada. `git_status.typescript` conserva el estado observado al capturar metadata; archivos de evidencia agregados despues pertenecen al mismo run set.

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

No se instalaron ni sustituyeron paquetes durante Milestone 2B.

## Configuracion inspeccionada

- `processed_project.yml`: configuracion normal resuelta.
- `processed_coverage.yml`: perfil coverage resuelto.
- `ceedling_source_inventory.typescript`: unico source declarado, `components/ads1263/src/ads1263.c`.
- Salidas separadas: `build/ceedling/normal` y `build/ceedling/coverage`.
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

Los transcripts de build muestran `ads1263.c`, ambos mocks generados y `wait_wrapper.c`; no se compilaron objetos de `components/ads1263/arch/linux/src/`.

## Guarda de produccion

Comandos ejecutados contra el baseline:

```sh
git diff --exit-code c59b24017f5daebb4e37512d799a0d07f589bce5 -- 'components/**/*.c' 'components/**/*.h' 'main/*.c' Makefile
GIT_PAGER=cat git diff --name-only c59b24017f5daebb4e37512d799a0d07f589bce5 -- 'components/**/*.c' 'components/**/*.h' 'main/*.c' Makefile
```

Ambos terminaron con exit `0` y sin nombres de archivo. Los transcripts son `production_guard_diff.typescript` y `production_guard_names_no_pager.typescript`; el primer intento de listado, `production_guard_names.typescript`, solo conserva secuencias de control del pager y tampoco contiene nombres. No se agregaron copias de algoritmos, ramas `UNIT_TEST`, exposicion de helpers static, cambios C/H de produccion ni cambios al `Makefile`.

## Auditoria de trazabilidad

`traceability_audit_pass.typescript` compara los 53 IDs nuevos extraidos de las cuatro suites extendidas con el catalogo: 53 IDs unicos, ninguno ausente y ninguno sin estado `PASS`. El intento previo `traceability_audit.typescript` uso un literal con backticks que el shell interpreto como sustitucion de comando; se conservo el fallo de auditoria y se corrigio el comando sin modificar tests ni resultados.

## Validacion final

| Control | Resultado | Evidencia raw |
|---|---|---|
| `openspec validate tsse-embedded-software-testing --strict` | Exit 0; change valido | `final_openspec_validate.typescript` |
| `GIT_PAGER=cat git diff --check` | Exit 0; sin diagnosticos | `final_git_diff_check_no_pager.typescript` |
| Whitespace en 19 archivos authored, incluidos untracked | Exit 0; 0 hallazgos | `final_authored_whitespace.typescript` |
| JSON de tests, cobertura y comparacion con `jq empty` | Exit 0; sin diagnosticos | `final_json_validation.typescript` |
| Guarda final de produccion | Exit 0; sin diferencias ni nombres | `final_production_guard_diff.typescript`; `final_production_guard_names.typescript` |
| Estado final del worktree | Capturado | `final_git_status.typescript` |

`final_git_diff_check.typescript` conserva un primer control exitoso que emitio solo secuencias del pager; se repitio con `GIT_PAGER=cat` para obtener una evidencia vacia y legible. `docs/tsse/evidencias/milestone_2b_checksums_final.typescript` registra SHA-256 de los artefactos del run set y documentos/test inputs finales; el transcript `milestone_2b_checksums.typescript` conserva la captura inmediatamente anterior al control adicional de whitespace.
