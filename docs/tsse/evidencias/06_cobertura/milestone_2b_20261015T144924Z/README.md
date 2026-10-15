# Cobertura extendida ADS1263 - Milestone 2B

## Run identificado

- Suites instrumentadas: las cinco suites host ADS1263, 62 tests en total.
- Perfil: `test/ceedling/mixins/coverage.yml`.
- Config gcovr: `test/ceedling/gcovr.cfg`.
- Compilador/gcov: GCC/gcov 15.2.0.
- Analizador: gcovr 7.2.
- SUT y unico archivo incluido en el reporte: `components/ads1263/src/ads1263.c`.
- Excluidos por filtro: tests, runners, mocks, Unity, CMock y soporte.
- No se definio ni aplico un umbral de aceptacion.

## Comandos y resultado

```sh
ceedling clobber --mixin=test/ceedling/mixins/coverage.yml
ceedling gcov:all --mixin=test/ceedling/mixins/coverage.yml
gcovr --root . --filter 'components/ads1263/src/ads1263\.c$' --merge-mode-functions merge-use-line-max --txt-summary --json-summary docs/tsse/evidencias/06_cobertura/milestone_2b_20261015T144924Z/coverage_summary.json
gcovr --root . --filter 'components/ads1263/src/ads1263\.c$' --merge-mode-functions merge-use-line-max --txt docs/tsse/evidencias/06_cobertura/milestone_2b_20261015T144924Z/coverage.txt --html-details docs/tsse/evidencias/06_cobertura/milestone_2b_20261015T144924Z/coverage.html --json docs/tsse/evidencias/06_cobertura/milestone_2b_20261015T144924Z/coverage.json --json-pretty --print-summary
```

Todos los comandos terminaron con exit `0`. El run instrumentado reporto 62 tests, 62 `PASS`, 0 `FAIL`, 0 `IGNORED`. Los transcripts son `01_coverage_clobber.typescript`, `02_coverage_run.typescript`, `03_gcovr_summary.typescript` y `04_gcovr_artifacts.typescript`.

## Resultado y comparacion

| Medida | Milestone 2A | Milestone 2B | Delta cubierto | Delta porcentual |
|---|---:|---:|---:|---:|
| Lineas | 13/222 (5.9%) | 151/222 (68.0%) | +138 | +62.1 pp |
| Funciones | 2/16 (12.5%) | 13/16 (81.2%) | +11 | +68.7 pp |
| Branches | 2/134 (1.5%) | 90/134 (67.2%) | +88 | +65.7 pp |

Los denominadores permanecieron iguales. `coverage_comparison.json` conserva la comparacion machine-readable; el baseline original sigue en `../milestone_2a_20261015T142406Z/coverage_summary.json`.

## Pendiente visible

Las funciones sin ejecucion son `ads1263_close`, `ads1263_init` y `ads1263_mock_registers_reset`, segun `05_uncovered_functions.typescript`. La cobertura restante no convierte casos diferidos en ejecutados ni establece conformidad de requisitos fuera del alcance de las pruebas atomicas.

Artefactos: `coverage.txt`, `coverage.json`, `coverage_summary.json`, `coverage_comparison.json`, `coverage.html`, `coverage.functions.html`, detalle HTML de `ads1263.c`, `coverage.css`, JSON/JUnit del run instrumentado y transcripts raw.
