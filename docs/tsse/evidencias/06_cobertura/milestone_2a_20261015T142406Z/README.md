# Cobertura inicial ADS1263 - Milestone 2A

## Run identificado

- Suite instrumentada: `test/ceedling/tests/test_ads1263_initial.c`.
- Perfil: `test/ceedling/mixins/coverage.yml`.
- Config gcovr: `test/ceedling/gcovr.cfg`.
- Compilador/gcov: GCC/gcov 15.2.0.
- Analizador: gcovr 7.2.
- SUT y unico archivo incluido en el reporte: `components/ads1263/src/ads1263.c`.
- Excluidos por filtro: tests, runners, mocks, Unity, CMock y soporte.
- No se aplico umbral de aceptacion.

## Comandos y resultado

```sh
ceedling clobber --mixin=test/ceedling/mixins/coverage.yml
ceedling gcov:all --mixin=test/ceedling/mixins/coverage.yml
gcovr --root . --filter 'components/ads1263/src/ads1263\.c$' --merge-mode-functions merge-use-line-max --txt-summary --json-summary docs/tsse/evidencias/06_cobertura/milestone_2a_20261015T142406Z/coverage_summary.json
gcovr --root . --filter 'components/ads1263/src/ads1263\.c$' --merge-mode-functions merge-use-line-max --txt docs/tsse/evidencias/06_cobertura/milestone_2a_20261015T142406Z/coverage.txt --html-details docs/tsse/evidencias/06_cobertura/milestone_2a_20261015T142406Z/coverage.html --json docs/tsse/evidencias/06_cobertura/milestone_2a_20261015T142406Z/coverage.json --json-pretty --print-summary
```

El run instrumentado termino con exit `0`: 9 tests, 9 `PASS`, 0 `FAIL`, 0 `IGNORED`. Los transcripts exitosos son `04_coverage_run_pass.typescript`, `05_gcovr_summary.typescript` y `07_gcovr_artifacts_pass.typescript`.

## Baseline medido

| Archivo de produccion | Lineas | Funciones | Branches |
|---|---:|---:|---:|
| `components/ads1263/src/ads1263.c` | 13/222 (5.9%) | 2/16 (12.5%) | 2/134 (1.5%) |

Artefactos: `coverage.txt`, `coverage.json`, `coverage_summary.json`, `coverage.html`, `coverage.functions.html`, `coverage.ads1263.c.1c66117f7ec3357b063c31f9de40a3a3.html` y `coverage.css`.

## Interpretacion

Las dos funciones seleccionadas fueron ejecutadas: `ads1263_default_config` una vez y `ads1263_raw_to_voltage` nueve veces. Las dos salidas instrumentadas del ternario `gain == 0U` fueron tomadas (8 veces gain no-cero y una vez gain cero). Los otros 14 simbolos de la unidad de traduccion no se ejecutaron y quedan diferidos a tareas 7.2-7.5; por eso el porcentaje de modulo completo es deliberadamente bajo y no se redefine como fallo.

## Incidencias de reporte preservadas

- `02_coverage_run.typescript`: tests instrumentados pasaron, pero gcovr rechazo el encabezado INI de la configuracion; se corrigio a sintaxis key-value.
- `06_gcovr_artifacts.typescript`: invocacion de export manual mal formada por ubicar el path tras `--json-pretty`; se corrigio usando `--json <path> --json-pretty`.

Ambas incidencias pertenecen a infraestructura/reporte, no a produccion. Sus evidencias no fueron sobrescritas.
