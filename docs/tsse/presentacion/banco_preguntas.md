# Banco de preguntas

## Alcance y arquitectura

### 1. ¿Por qué se afirma “host-only” si se compila código de producción?

Porque el SUT C es real, pero SPI, GPIO, tiempo y otras fronteras se controlan o simulan. Esto verifica contratos software, no fenómenos físicos.

**Evidencia:** `docs/architecture.md`; `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json`, `limitations`.

### 2. ¿Se probó la placa STM32MP157D-DK1?

No en la campaña TSSE actual. La validación de placa es un antecedente histórico informado y no tiene raw log adjunto reproducible en este repositorio.

**Evidencia:** `docs/tsse/discrepancias.md`, `DSP-016`; `docs/tsse/evidencias/01_proyecto/milestone_1_linea_base.md`.

### 3. ¿Hubo cambios de producción para obtener estos resultados?

No. El alcance declarado conserva producción sin cambios y usa soporte de test, documentación y evidencia.

**Evidencia:** `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md`; `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md`.

## CFT y pruebas unitarias

### 4. ¿Por qué 24 paths si la complejidad ciclomática es 33?

`V(G)=33` es la dimensión estructural del grafo normalizado, no la cantidad de paths factibles desde la raíz fija. Hay edges inviables por punteros no nulos, `live=0`, validación previa y loops o retries no acotados. Los 24 paths seleccionan outcomes factibles relevantes; no se afirma “all paths”.

**Evidencia:** `docs/tsse/cft_acquisition_run_samples.md`, “Complejidad”, “Restricciones” y “Selección”; `docs/tsse/cft_cases.md`.

### 5. ¿Qué demuestra CFT-P21?

Con `N=2`, `rate=2`, canal 0 y sin CSV: preparación exitosa, dos reads y outputs, un sleep, un back edge, cierre y retorno 0.

**Evidencia:** `docs/tsse/cft_cases.md`, fila `CFT-P21`.

### 6. ¿Por qué P21 no usa rate 1?

Porque `rate=1` con más de una muestra activa DEF-001: se construye `tv_nsec=1000000000`, POSIX devuelve `EINVAL` y el retry no progresa. Rate 2 permite un path finito válido.

**Evidencia:** reporte `DEF-001-DSP017-rate-1-livelock.md`.

### 7. ¿Cómo se obtienen las 72 unitarias no-CFT?

Son 66 casos ADS1263 más 6 casos de robustez/caracterización acquisition. Los 24 casos `test_CFT_*` se informan aparte.

**Evidencia:** `ads1263_tests_report.json` reporta 66; `acquisition_tests_report.json` contiene 30, de los cuales 24 son CFT y 6 no-CFT.

### 8. ¿Los mocks pueden ocultar defectos?

Sí. Por eso las afirmaciones se limitan al contrato simulado y se complementan con integración host, probes POSIX, cobertura, estático y una lista explícita de limitaciones. Los mocks no sustituyen hardware.

**Evidencia:** `docs/tsse/master_test_plan.md`; `run_1/summary.json`, `limitations`.

## Integración y resultados

### 9. ¿Por qué dos runs de 71 escenarios?

Para demostrar repetibilidad formal. Cada run conserva resumen y evidencia por caso; ambos declaran 71, ejecutan 71 y obtienen 71 PASS.

**Evidencia:** `run_1/summary.json` y `run_2/summary.json`.

### 10. ¿Por qué algunos casos PASS tienen exit code 1?

Porque son escenarios negativos de CLI o HAL. PASS significa que el exit code y los mensajes observados coinciden con el oráculo esperado, no que todo proceso deba retornar cero.

**Evidencia:** `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/02_run_1.typescript` y resultados por caso.

### 11. ¿Qué cubren los 71 escenarios?

Parsing, valores ausentes/malformados/límites, precedencia de modos, mock acquisition, CSV, observación con `strace`, señal SIGINT y escenarios fake-HAL de éxito y error.

**Evidencia:** `run_1/scenarios.md`, `run_2/scenarios.md` y sus `summary.json`.

### 12. ¿Se prueba libgpiod en integración?

No. Los escenarios fake-HAL ejercitan framing y orquestación, pero no el comportamiento real de `ioctl` o libgpiod.

**Evidencia:** `run_1/summary.json`, segunda limitación.

## Cobertura y trazabilidad

### 13. ¿La cobertura de 68–70% es suficiente?

No se definió un umbral de suficiencia. La cobertura es un indicador de observación y una guía de gaps, no una prueba de ausencia de defectos.

**Evidencia:** `docs/tsse/evidencias/06_cobertura/milestone_3a_20261015T150616Z/README.md`; `docs/tsse/master_test_plan.md`.

### 14. ¿Por qué no se presenta una cobertura global?

Porque los módulos tienen denominadores y suites distintas. Se preservan líneas, funciones y ramas por módulo para evitar una agregación engañosa.

**Evidencia:** `GcovCoverage.json` y `coverage.txt` de cada módulo.

### 15. ¿Qué explica la mejora de acquisition?

La medición previa usaba los 6 casos de robustez; la actual incorpora los 24 paths CFT. Pasa de `54/168`, `6/13`, `26/76` a `118/168`, `9/13`, `53/76`.

**Evidencia:** README de cobertura Milestone 3A y `acquisition/GcovCoverage.json` de Milestone 4.

### 16. ¿Cómo se rastrea una afirmación a evidencia?

Mediante requisito o riesgo, ID de caso, oráculo, estado y ruta de evidencia en la matriz y el catálogo; los slides y notas enlazan las rutas relevantes.

**Evidencia:** `docs/tsse/matriz_trazabilidad.md`; `docs/tsse/catalogo_casos.md`; `docs/tsse/cft_cases.md`.

## Estático, riesgos y defecto

### 17. ¿Cppcheck encontró 41 defectos?

No. Encontró 41 diagnósticos source de severidad `style`: 37 `unusedStructMember` por contexto de headers/configuración, 1 `constParameterPointer` de contexto/tool y 3 `staticFunction` aceptados como observaciones de interfaz pública. Los 32 notices `missingIncludeSystem` se informan aparte y no integran esos 41. El análisis confirmó cero defectos nuevos.

**Evidencia:** `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md`; `findings.md`.

### 18. ¿Por qué la rama libgpiod está BLOCKED y no FAIL?

Porque el análisis no pudo comenzar: faltaban `pkg-config` y `gpiod.h`. No hubo una ejecución que produjera un resultado de conformidad adverso. Tampoco puede llamarse clean.

**Evidencia:** README estático, “Configuration gate”; `raw_analysis/002_cppcheck_libgpiod_v2.*`.

### 19. ¿Cómo se confirmó DEF-001?

Con tres niveles: captura unitaria del `timespec`, llamada POSIX host que devuelve `EINVAL`, y probe de proceso acotado que no termina antes de dos segundos y sale 124 por timeout.

**Evidencia:** `DEF-001-DSP017-rate-1-livelock.md`; `03_dsp017_probe_timeout.typescript`.

### 20. ¿Por qué ROB-001 no es un defecto confirmado?

El efecto inseguro intraobjeto está observado, pero no existe un contrato aprobado que exija un status específico para el rango combinado. Se conserva como `SUSPECTED DEFECT`; el PASS de ASan/UBSan no demuestra seguridad.

**Evidencia:** README de defectos/regresión, filas ROB-001 y sección “Hallazgo de sanitizer”.

### 21. ¿Qué falta resolver en ROB-003?

Definir si un error o timeout DRDY con transferencia SPI exitosa debe ser fatal o sólo diagnóstico. El comportamiento y sus consecuencias están caracterizados, pero el contrato normativo sigue ambiguo.

**Evidencia:** README de defectos/regresión, filas ROB-003.

### 22. ¿Cuál sería el siguiente paso más importante?

Un gate humano para corregir DEF-001 en un cambio separado, seguido por regresión completa. Después, habilitar libgpiod y ejecutar una validación de placa reproducible con raw logs.

**Evidencia:** reporte DEF-001, sección “Regresión futura”; README estático; `DSP-016`.
