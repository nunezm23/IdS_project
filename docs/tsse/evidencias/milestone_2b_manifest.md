# Manifest de evidencia - Milestone 2B

Run set: `milestone_2b_20261015T144924Z`.

| Actividad/casos | Estado | Evidencia principal | Raw/machine-readable |
|---|---|---|---|
| Entorno, versiones y procedencia | `PASS` | `01_proyecto/milestone_2b_20261015T144924Z/README.md` | Transcripts, checksums y `processed_*.yml` en el mismo directorio |
| RUN-DET-001, suite normal completa en dos runs limpios | `PASS` | `04_unitarias/milestone_2b_20261015T144924Z/README.md` | `02_unit_run_1.typescript`; `04_unit_run_2.typescript`; JSON/JUnit |
| UT-REG-001..006, 009..011 | `PASS` | `04_unitarias/milestone_2b_20261015T144924Z/README.md#resultados-atomicos-nuevos` | JSON/JUnit y ambos runs raw |
| UT-PRES-001..011 | `PASS` | `04_unitarias/milestone_2b_20261015T144924Z/README.md#resultados-atomicos-nuevos` | JSON/JUnit y ambos runs raw |
| UT-CHAN-001..007 | `PASS` | `04_unitarias/milestone_2b_20261015T144924Z/README.md#resultados-atomicos-nuevos` | JSON/JUnit y ambos runs raw |
| UT-SAMPLE-001..008 | `PASS` | `04_unitarias/milestone_2b_20261015T144924Z/README.md#resultados-atomicos-nuevos` | JSON/JUnit y ambos runs raw |
| UT-DRDY-001..005 | `PASS` | `04_unitarias/milestone_2b_20261015T144924Z/README.md#observaciones-de-caracterizacion` | JSON/JUnit y ambos runs raw |
| ANA-COV-001 actualizado | `PASS` | `06_cobertura/milestone_2b_20261015T144924Z/README.md` | Resumen, comparacion, JSON, HTML y transcripts |
| Guarda de produccion | `PASS` | `01_proyecto/milestone_2b_20261015T144924Z/README.md#guarda-de-produccion` | `production_guard_diff.typescript`; `production_guard_names_no_pager.typescript` |
| Validacion final OpenSpec/whitespace/JSON/produccion | `PASS` | `01_proyecto/milestone_2b_20261015T144924Z/README.md#validacion-final` | Transcripts `final_*` en `01_proyecto/`; `milestone_2b_checksums_final.typescript` |

El manifest de Milestone 2A se conserva sin reemplazo. Los casos excluidos en el README de pruebas mantienen `NOT RUN`; este manifest no infiere resultados desde cobertura ni confirma defectos para comportamientos sin contrato aprobado.
