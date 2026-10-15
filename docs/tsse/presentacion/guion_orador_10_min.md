# Guion del orador - 10 minutos

## Uso

- Duración objetivo exacta: `10:00`.
- Láminas principales: 1 a 9. Las láminas 10 a 16 son respaldo y no forman parte del tiempo objetivo.
- Regla verbal: usar “host-only”, “histórico”, “BLOCKED” y “defecto confirmado” con el significado preciso indicado aquí.
- Las notas resumidas también están embebidas en el `.pptx`; este archivo es la versión detallada.

## 1. Verificación de software para adquisición ADS1263 - 0:35

**Objetivo temporal:** `00:00–00:35`.

“Este trabajo construye evidencia reproducible para el software de adquisición ADS1263. La presentación separa tres tipos de ejecución: 72 unitarias que no son CFT, 24 paths CFT y 71 escenarios de integración repetidos en dos corridas formales. El resultado actual es exclusivamente host-only: no se usó hardware y no se modificó producción. También preservamos un resultado adverso importante: DEF-001 es el único defecto confirmado y sigue abierto.”

**Énfasis:** no sumar las dos corridas de integración como casos únicos y no ocultar DEF-001 detrás del total PASS.

**Evidencia:**

- `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/ads1263_tests_report.json`
- `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json`
- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json`
- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_2/summary.json`
- `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md`

## 2. Sistema, baseline y frontera - 1:05

**Objetivo temporal:** `00:35–01:40`.

“El flujo del sistema va desde la CLI a acquisition, luego al core ADS1263, la HAL Linux y finalmente la placa. La evidencia actual cruza CLI, orquestación y core, y usa límites controlados para SPI, GPIO, tiempo, logger y filesystem según el nivel de prueba. Esto permite verificar contratos software reales sin depender del laboratorio.”

“La frontera importa: no verificamos buses físicos, comportamiento eléctrico, exactitud ADC ni tiempo real del target. La rama real de libgpiod tampoco fue analizada porque faltaban sus prerrequisitos. Existe una validación previa en STM32MP157D-DK1 y ADS1263, pero está explícitamente rotulada como **validación histórica de placa**: el repositorio no contiene su raw log, por lo que no es un resultado TSSE reproducible.”

“También separo la provenance de snapshots: el diseño CFT modela el baseline de producción `c59b24017f5daebb4e37512d799a0d07f589bce5`; el análisis estático Milestone 4 registra HEAD `54a91ed4b684b5ac6f4221c8743768fad5083270`. No se presentan como el mismo estado.”

**Frase de cierre:** “La presentación distingue contexto del proyecto de evidencia ejecutada.”

**Evidencia:**

- `docs/architecture.md`
- `docs/tsse/cft_acquisition_run_samples.md`
- `docs/tsse/discrepancias.md` (`DSP-016`)
- `docs/tsse/evidencias/01_proyecto/milestone_1_linea_base.md`
- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json`, campo `limitations`
- `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md`, secciones “Configuration gate” y “Limitations”

## 3. Estrategia de calidad por capas - 1:00

**Objetivo temporal:** `01:40–02:40`.

“No usamos un único porcentaje como señal de calidad. Partimos de requisitos, riesgos y casos trazables. Las unitarias aíslan contratos; el CFT deriva casos desde decisiones y edges; integración recorre CLI, CSV, procesos y una HAL controlada; cobertura muestra qué código quedó observado; y el análisis estático obliga a disponer cada diagnóstico.”

“Los oráculos no se limitan al exit code: verifican retorno, secuencia, conteos, contenido, cierre de recursos y archivos. Además, cada afirmación numérica conserva una ruta a raw o a un resumen derivado. La semántica también es parte de la calidad: PASS se limita al entorno declarado y BLOCKED nunca se presenta como clean.”

**Evidencia:**

- `docs/tsse/master_test_plan.md`
- `docs/tsse/matriz_trazabilidad.md`
- `docs/tsse/catalogo_casos.md`
- `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/00_policy.md`

## 4. CFT: ejemplo P21 - 1:30

**Objetivo temporal:** `02:40–04:10`.

“El modelo CFT toma `acquisition_run_samples()` como raíz y expande los helpers locales de acquisition a profundidad uno. Deja ADS1263, logger, archivo y tiempo como contratos externos controlables. El grafo normalizado tiene 32 decisiones y complejidad ciclomática 33. A partir de restricciones de factibilidad se seleccionaron 24 paths ejecutables; no se afirma cobertura de todos los paths.”

“P21 es el ejemplo más legible. La entrada usa dos muestras, rate 2 y canal 0. Primero pasa validación; luego init, presence y configure; produce la primera muestra; ejecuta exactamente un sleep y un back edge; produce la segunda muestra y cierra. El oráculo exige retorno 0, dos reads, dos pares de output, un sleep y un close.”

“Rate 2 es intencional. Con rate 1 y más de una muestra aparece DEF-001: el `timespec` es inválido y el retry no progresa. Por eso ese comportamiento no se fuerza dentro de un path finito exitoso; se investiga por separado como defecto.”

**Evidencia:**

- `docs/tsse/cft_acquisition_run_samples.md`, decisiones, factibilidad y selección
- `docs/tsse/cft_cases.md`, `CFT-P21`
- `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json`
- `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/04_acquisition_cft_clean_test_all_after_fix.typescript`

## 5. Arquitectura unitaria y mocks - 1:05

**Objetivo temporal:** `04:10–05:15`.

“La unidad bajo prueba es código de producción real: `ads1263.c` y `acquisition.c` se compilan en host. Alrededor se controlan las dependencias con CMock, wrappers de stdio y espera, logger spy y scripts de outcomes. Ese diseño hace deterministas los errores y permite observar orden, conteos, muestras, CSV y cleanup.”

“El inventario se separa para evitar dobles conteos: ADS1263 tiene 66 casos. Acquisition aporta 6 casos no-CFT de robustez y caracterización. Esos dos grupos forman las 72 unitarias no-CFT. En el mismo proyecto acquisition se ejecutan además 24 casos CFT, P01 a P24.”

**Evidencia:**

- `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/ads1263_tests_report.json`
- `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json`
- `test/ceedling/project_acquisition_robustness.yml`
- `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md`

## 6. Resultados actuales - 1:05

**Objetivo temporal:** `05:15–06:20`.

“Los resultados formales son: 72 de 72 unitarias no-CFT PASS; 24 de 24 CFT PASS; y 71 de 71 escenarios de integración PASS en cada una de dos corridas. Ambas corridas reportan cero fallos, cero bloqueados y coincidencia exacta entre escenarios declarados y ejecutados.”

“La integración cubre parsing y precedencia CLI, creación y truncado CSV, errores de archivo, señales, observaciones de proceso y escenarios fake-HAL. Sigue siendo host-only: no ejercita SPI o GPIO físicos, exactitud ADC ni timing de target. Durante este alcance no se modificó producción y no se accedió a hardware.”

**Evidencia:**

- `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/`
- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json`
- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_2/summary.json`
- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/02_run_1.typescript`
- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/03_run_2.typescript`

## 7. Cobertura y trazabilidad - 1:10

**Objetivo temporal:** `06:20–07:30`.

“La cobertura se informa por módulo y por denominador. En `ads1263.c`: 151 de 222 líneas, 13 de 16 funciones y 90 de 134 ramas. En `acquisition.c`: 118 de 168 líneas, 9 de 13 funciones y 53 de 76 ramas.”

“La referencia previa para acquisition era 54 de 168 líneas, 6 de 13 funciones y 26 de 76 ramas. El crecimiento acompaña la ejecución CFT, pero no se convierte en una garantía ni en un umbral inventado. Los huecos siguen visibles.”

“La trazabilidad conecta requisito o riesgo, ID de caso, oráculo, estado y evidencia. Eso permite responder no sólo cuánto se ejecutó, sino por qué existe cada caso y qué afirmación soporta.”

**Evidencia:**

- `docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/ads1263/GcovCoverage.json`
- `docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/acquisition/GcovCoverage.json`
- `docs/tsse/evidencias/06_cobertura/milestone_3a_20261015T150616Z/README.md`
- `docs/tsse/matriz_trazabilidad.md`
- `docs/tsse/catalogo_casos.md`

## 8. Hallazgos, estático y DEF-001 - 1:10

**Objetivo temporal:** `07:30–08:40`.

“Cppcheck produjo 41 hallazgos source de severidad style: 37 `unusedStructMember` por contexto de headers/configuración, un `constParameterPointer` de contexto/tool y tres `staticFunction` aceptados como interfaz pública. Los 32 notices `missingIncludeSystem` se informan por separado y no integran esos 41. No hubo diagnósticos error, warning, performance o portability, y el análisis estático no confirmó defectos de producción nuevos.”

“La rama `ADS1263_NO_GPIOD=0` quedó BLOCKED: no estaban disponibles `pkg-config` ni `gpiod.h`. No se instaló nada y esa rama no se describe como clean.”

“DEF-001 es distinto y es el único defecto confirmado. Para rate 1 se construye `tv_nsec=1000000000`; POSIX responde EINVAL y el mismo valor se reintenta sin progreso. Un probe acotado con dos muestras termina por timeout externo, exit 124. ROB-001 conserva clasificación de defecto sospechado y ROB-003 de ambigüedad contractual.”

**Evidencia:**

- `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md`
- `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/findings.md`
- `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md`
- `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md`

## 9. Conclusión y límites - 1:20

**Objetivo temporal:** `08:40–10:00`.

“La conclusión es deliberadamente acotada. Podemos defender contratos unitarios repetibles, 24 paths CFT ejecutados, integración host repetida, cobertura modular trazable y una disposición completa de hallazgos. No podemos defender comportamiento eléctrico, buses físicos, exactitud ADC, tiempo real de target ni la rama libgpiod bloqueada.”

“Los tres próximos pasos son concretos y todavía no ejecutados: primero, decisión humana y cambio separado para corregir DEF-001; segundo, habilitar el entorno libgpiod y repetir el análisis de esa configuración; tercero, una validación de placa nueva con comandos, metadata y raw logs preservados. Hasta entonces, la validación de placa sólo puede citarse como antecedente histórico.”

“El valor de este trabajo no es declarar el sistema perfecto, sino dejar claro qué sabemos, cómo lo reproducimos y qué queda abierto.”

**Evidencia:**

- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json`, `limitations`
- `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md`, `Limitations`
- `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md`
- `docs/tsse/discrepancias.md`, `DSP-016`

## Respaldo 10. Todos los grupos CFT

Usar para explicar cómo P01–P24 se agrupan en validación, inicialización, presencia, configuración, CSV, lectura, loop/sleep y límite superior. No incluir en los 10 minutos salvo interrupción.

**Evidencia:** `docs/tsse/cft_cases.md` y `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json`.

## Respaldo 11. Inventario de pruebas

Usar para distinguir casos únicos, repeticiones y probe de conformidad. El FAIL del probe es el resultado esperado para demostrar DEF-001, no una falla de infraestructura.

**Evidencia:** reportes JSON unitarios, resúmenes de integración y `docs/tsse/evidencias/milestone_3a_completion_report.md`.

## Respaldo 12. Extractos raw

Usar para mostrar salidas primarias. Advertir que son extractos tipografiados y dirigir al archivo raw completo.

**Evidencia:**

- `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/04_acquisition_cft_clean_test_all_after_fix.typescript`
- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/02_run_1.typescript`
- `docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/acquisition/GcovCoverage.json`
- `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/03_dsp017_probe_timeout.typescript`

## Respaldo 13. Cobertura detallada

Usar si preguntan porcentajes o denominadores. No producir una cobertura global combinada.

**Evidencia:** `GcovCoverage.json` por módulo y README de cobertura Milestone 3A.

## Respaldo 14. Disposiciones estáticas

Usar para separar los 41 source findings (`37 unusedStructMember + 1 constParameterPointer + 3 staticFunction`) de las 32 limitaciones `missingIncludeSystem`, los dos notices de profundidad cerrados y un blocker de configuración.

**Evidencia:** `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/findings.md`.

## Respaldo 15. ROB-001/003 y DEF-001

Usar para defender la taxonomía. Sólo DEF-001 es defecto confirmado.

**Evidencia:** `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` y reporte DEF-001.

## Respaldo 16. Mapa de evidencia

Usar como índice durante preguntas o revisión. Las rutas son relativas a la raíz del repositorio.

**Evidencia de la presentación:** `docs/tsse/evidencias/09_presentacion/milestone_4_20261015T161203Z/README.md`.

## Control temporal

| Lámina | Duración | Acumulado |
|---|---:|---:|
| 1 | 0:35 | 0:35 |
| 2 | 1:05 | 1:40 |
| 3 | 1:00 | 2:40 |
| 4 | 1:30 | 4:10 |
| 5 | 1:05 | 5:15 |
| 6 | 1:05 | 6:20 |
| 7 | 1:10 | 7:30 |
| 8 | 1:10 | 8:40 |
| 9 | 1:20 | 10:00 |
