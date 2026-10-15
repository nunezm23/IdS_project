# Master Test Plan — TSSE

## 1. Proyecto

Sistema de adquisición de datos basado en STM32MP157D-DK1
y conversor analógico-digital ADS1263.

El software original está implementado en C para Linux embebido.

## 2. Objetivo del testing

Diseñar y ejecutar pruebas automatizadas sobre módulos
de software existentes, evaluando el cumplimiento de los
requisitos mediante pruebas unitarias, pruebas de integración
en host y técnicas formales de generación de casos.

## 3. Alcance

Incluye:
- Driver ADS1263.
- Control de adquisición.
- Validación de parámetros.
- Conversión de muestras.
- Tratamiento de errores de software.
- Interacciones seleccionadas entre módulos.

Excluye:
- Validación eléctrica del ADS1263.
- Medición de señales SPI físicas.
- Pruebas con potenciómetro.
- Compilación cruzada y despliegue en STM32MP157D-DK1.
- Caracterización temporal del hardware real.

## 4. Características de calidad propuestas

| Característica | Prioridad |
|---|---:|
| Funcionalidad | 45 % |
| Confiabilidad | 30 % |
| Mantenibilidad | 15 % |
| Eficiencia | 10 % |
| **Total** | **100 %** |

## 5. Estrategia

- Análisis estático: Cppcheck.
- Diseño formal de casos de sistema: CFT.
- Pruebas unitarias: Ceedling + Unity.
- Aislamiento de dependencias: CMock.
- Cobertura de código: gcov/gcovr.
- Pruebas de integración: ejecución en host con dependencias simuladas.

## 6. Línea base

Commit original:
c59b24017f5daebb4e37512d799a0d07f589bce5

Rama de trabajo:
feature/tsse-testing

El código original ya fue validado funcionalmente
en la STM32MP157D-DK1 con un ADS1263.

Las nuevas pruebas no pretenden volver a validar el hardware.

## 7. Priorización por subsistema

| Subsistema | Importancia | Justificación |
|---|---:|---|
| Driver ADS1263 | 45 % | Conversión, registros, validación y errores |
| Adquisición | 35 % | Secuencias y control del muestreo |
| CLI / main | 10 % | Validación y selección de parámetros |
| Logger / CSV | 5 % | Presentación y exportación |
| Interfaces Linux SPI/GPIO | 5 % | Interacciones mediante mocks |
| **Total** | **100 %** | |

## 8. Características de calidad por nivel de prueba

Leyenda:
- ++: prioridad alta en este nivel.
- +: relevante.
- -: fuera del alcance de este nivel.

| Nivel | Funcionalidad | Confiabilidad | Mantenibilidad | Eficiencia |
|---|:---:|:---:|:---:|:---:|
| Análisis estático | + | + | ++ | + |
| Unitario | ++ | ++ | + | - |
| Integración de software en host | ++ | ++ | + | - |
| Sistema simulado | ++ | + | - | + |
| Integración HW/SW | - | - | - | - |

El análisis estático se incluye como actividad de verificación,
aunque no constituye por sí mismo un nivel de prueba dinámica.

La integración HW/SW queda fuera del alcance del TP.

## 9. Técnicas por subsistema

| Subsistema | Técnica | Herramienta |
|---|---|---|
| Driver ADS1263 | Valores límite y particiones de equivalencia | Ceedling / Unity |
| Driver ADS1263 | Pruebas de rutas de error con mocks | Ceedling / CMock |
| Adquisición | Control Flow Test (CFT) | Diagramas y casos documentados |
| CLI / main | Casos de entradas válidas e inválidas | Ejecución en host |
| Logger / CSV | Verificación de formato | Tests funcionales |
| Integración en host | Escenarios con dependencias simuladas | GCC / Ceedling |

## 10. Criterios de ejecución y aceptación

- Cada prueba tendrá identificador y resultado esperado.
- Se conservarán las salidas reales de ejecución.
- Las pruebas unitarias tendrán resultados PASS/FAIL.
- La cobertura se medirá sobre los módulos efectivamente ensayados.
- Los defectos confirmados se documentarán antes de corregirse.
- Después de una corrección se ejecutarán pruebas de regresión.
- Las pruebas no ejecutadas se identificarán explícitamente.
- Los resultados simulados no se presentarán como validación física.

No se establece todavía un porcentaje mínimo obligatorio
de cobertura; se evaluará a partir de los resultados reales.

## 11. Entorno y configuracion

| Elemento | Baseline aprobado |
|---|---|
| Ejecucion | Ubuntu 26.04 x86-64 |
| Compilador host | GCC 15.2 |
| Unit testing | Ceedling 1.1.10, Unity y CMock |
| Cobertura | gcov/gcovr |
| Analisis estatico | Cppcheck |
| Modelado CFT | Graphviz y documentacion de caminos |
| Hardware | No requerido ni utilizado para el TP |

El `Makefile` de produccion y el SDK del target no forman parte del build de tests. La disponibilidad observada de una herramienta no equivale a una ejecucion. Cada actividad conservara comando, version, revision, exit code y salida real cuando se ejecute.

## 12. Roles y responsabilidades

| Rol | Responsabilidad |
|---|---|
| Responsable de testing / alumno | Mantener plan, modelos, casos, automatizacion, evidencia y trazabilidad |
| Revisor tecnico | Contrastar modelos y oraculos contra fuente/requisitos; registrar disposicion |
| Aprobador humano | Aprobar contratos ambiguos y cualquier cambio futuro en produccion |
| Herramientas/agents | Asistir en inspeccion y generacion; sus salidas requieren revision y no sustituyen evidencia de ejecucion |

Una misma persona puede ocupar mas de un rol en el contexto academico, pero la revision debe quedar explicitamente registrada.

## 13. Niveles, limites y objetivos

| Nivel o actividad | SUT / limite | Objetivo | Tecnica principal | No demuestra |
|---|---|---|---|---|
| Inspeccion estatica | Todo C/H de produccion | Estructura, contratos, dependencias y findings | Revision + Cppcheck | Ejecucion dinamica |
| Diseno CFT de componente | `acquisition_run_samples()` y helpers locales | Derivar caminos factibles, limites y oraculos | CFG interprocedural acotado | Paths internos ADS1263/HAL |
| Unitario ADS1263 | `ads1263.c` real con HAL controlado | Conversion, protocolo, limites y errores | Particiones, valores limite, mocks | Comunicacion fisica |
| Unitario acquisition | `acquisition.c` real con ADS1263 controlado | Orden, errores, conteos y cleanup | Casos CFT, event trace | ADC/HAL interno |
| Integracion host | Modulos reales seleccionados con hardware simulado | CLI, flujo mock, CSV y contratos entre modulos | Escenarios de proceso | Target, electricidad, timing real |
| Integracion HW/SW | Fuera de alcance | Solo antecedente historico | No se ejecuta | No aplica |

Objetivos por subsistema:

| Subsistema | Objetivo verificable |
|---|---|
| ADS1263 core | Verificar transformaciones, tramas, decisiones y propagacion de estados mediante interfaces controladas |
| Acquisition | Verificar validacion, secuencia, loops acotados, salida y liberacion mediante el CFT |
| CLI/main | Verificar posteriormente gramatica, limites y codigos de salida en proceso host |
| Logger/CSV | Verificar formato y efectos de archivo sin confundir expected con observed |
| SPI/GPIO Linux | Verificar posteriormente argumentos/errores mediante doubles; nunca hardware fisico en este TP |

### Profundidad CFT

Se adopta profundidad 1 de componente: raiz `acquisition_run_samples()` con expansion de todos los helpers locales alcanzables de `acquisition.c`. ADS1263, logger, archivo y tiempo se representan como contratos externos. Una profundidad literal limitada al cuerpo de la funcion omitiria las decisiones relevantes; expandir driver y HAL mezclaria niveles y haria el modelo desproporcionado.

## 14. Criterios de entrada, salida y suspension

### Entrada a diseno

- Baseline y revision documental identificados.
- Fuente de produccion sin diferencias respecto del baseline protegido.
- Requisitos y discrepancias disponibles.
- Alcance y exclusiones aprobados.

### Entrada a ejecucion automatizada

- Modelo y casos revisados.
- Toolchain requerido disponible o actividad marcada `BLOCKED`.
- Doubles documentados y sin acceso a dispositivos fisicos.
- Comandos reproducibles y ubicacion de evidencia definidos.

### Salida de Milestone 1

- Auditoria, plan y trazabilidad revisados.
- CFG numerado en DOT, SVG y PNG.
- Decisiones, complejidad, factibilidad y caminos seleccionados documentados.
- Casos CFT concretos con estado `NOT RUN`.
- Guarda de produccion sin diferencias.

### Salida global

- Casos implementados y estados respaldados por evidencia.
- Cobertura por modulo informada sin umbral inventado.
- Findings y defectos con disposicion.
- Reporte y presentacion consistentes con resultados reales.

### Suspension y reanudacion

Se suspende una actividad si falta una herramienta obligatoria, el modelo contradice la fuente, un oraculo depende de contrato no aprobado, un proceso puede quedar sin limite, o aparece un cambio no autorizado en produccion. Se reanuda solo tras registrar y resolver el bloqueo, actualizar el plan si corresponde y preservar la evidencia previa.

## 15. Estados de trabajo y resultado

| Estado | Uso |
|---|---|
| `PASS` | Ejecucion realizada y resultado observado coincide con el esperado |
| `FAIL` | Ejecucion realizada y existe diferencia observada |
| `BLOCKED` | Actividad en alcance que no puede ejecutarse con el entorno/decision actual |
| `NOT RUN` | Caso disenado o implementado sin ejecucion respaldada |
| `OUT OF SCOPE` | Actividad excluida deliberadamente, con razon |

Al cierre de Milestone 1 los casos disenados quedaron `NOT RUN`; Milestone 4 actualiza `CFT-P01..P24` a `PASS` con evidencia de ejecucion. Un resultado esperado por si solo no es evidencia observada.

## 16. Flujo de defectos y regresion

1. Registrar una hipotesis sin llamarla defecto.
2. Crear reproduccion controlada y conservar evidencia previa.
3. Comparar contra un contrato explicito aprobado.
4. Clasificar como defecto, comportamiento, ambiguedad, riesgo aceptado o bloqueo.
5. Antes de tocar C/H o `Makefile`, realizar gate humano y definir regresion.
6. Si se aprueba una correccion en un cambio separado, conservar before/after y ejecutar la regresion relacionada.

Al cierre de Milestone 1, ROB-001 y ROB-003 seguian siendo hipotesis y no se habian ejecutado. Milestone 3A agrega caracterizacion controlada sin correccion: ROB-001 queda como defecto sospechado por efecto intraobjeto y ROB-003 como ambiguedad contractual. Milestone 4 reproduce esas seis pruebas junto con los 24 casos CFT, sin cambiar sus disposiciones. DSP-017 queda confirmado como DEF-001 contra REQ-008 y espera gate humano antes de cualquier cambio de produccion.

## 17. Politica de evidencia

- No borrar ni sobrescribir evidencia historica.
- Usar IDs estables y referencias relativas al repositorio.
- Registrar revision, worktree, host, version, comando, timestamps, exit code y tipo de dependencia.
- Conservar salida raw separada de la interpretacion.
- Diferenciar claramente diseno, expected, observed, historial y resultado actual.
- No publicar cobertura, PASS, screenshots ni defectos sin ejecucion respaldatoria.
- Etiquetar todo resultado host como software-only/simulado cuando corresponda.

Ubicaciones de Milestone 1:

- Baseline e inventario: `docs/tsse/evidencias/01_proyecto/`.
- Modelo y graficos CFT: `docs/tsse/evidencias/03_cft/`.
- Plan, trazabilidad, discrepancias y casos: `docs/tsse/`.

## 18. Plan incremental

| Milestone | Entregable | Gate |
|---|---|---|
| 1 | Auditoria, MTP, trazabilidad, CFG y casos CFT | Revision humana antes de Ceedling |
| 2 | Infraestructura Ceedling y unit tests | Fuente real y produccion intacta |
| 3 | Robustez, regresion, cobertura e integracion host | Contrato aprobado antes de correcciones |
| 4 | Analisis estatico, cierre de evidencia, reporte y slides | Auditoria factual final |

No se asignan fechas academicas no provistas. El orden expresa dependencias, no duraciones comprometidas.

Milestone 4 completo la ejecucion tecnica y la auditoria documental del run `milestone_4_20261015T161203Z`. La aprobacion humana del reporte y el ensayo de presentacion no se consideran completados.

## 19. Riesgos y mitigaciones

| Riesgo | Mitigacion |
|---|---|
| Confundir mock con hardware | Etiquetar limite y no cerrar requisitos fisicos |
| Convertir comportamiento observado en requisito | Separar conformidad, caracterizacion e hipotesis |
| Sleeps o signals no deterministas | Wrappers/procesos acotados en milestones posteriores |
| Sobrescribir evidencia | Artefactos nuevos, IDs de run y enlaces raw |
| Inflar cobertura | Excluir framework/mocks y declarar denominador |
| Tool/version drift | Registrar version real y marcar bloqueos |
| Modelo CFT incorrecto | Revision independiente fuente-a-grafo |
| Exceso de contenido para 10 minutos | Presentar arquitectura, alcance, un CFG/camino, resultados y limites; detalles en backup |

## 20. Tratamiento de eficiencia

La prioridad historica de eficiencia permanece en 10 %, pero no se medira rendimiento temporal del target. En host solo se podran observar indicadores de software acotados, por ejemplo: ausencia de espera despues de la ultima muestra fija, cantidad esperada de llamadas de pacing y terminacion dentro de un timeout de seguridad. Estos indicadores sirven para control de flujo y determinismo; no permiten concluir frecuencia real, latencia SPI/GPIO ni cumplimiento real-time en STM32MP157D-DK1.

## 21. Estado actual

- Identidad de ejecucion: HEAD `54a91ed4b684b5ac6f4221c8743768fad5083270` mas inputs test/infraestructura dirty identificados por `evidencias/01_proyecto/milestone_4_20261015T161203Z/test_input_checksums.typescript`; HEAD solo no reproduce el run. El manifest es post-ejecucion sobre inputs sin cambios, no un commit pre-ejecucion. Baseline de produccion protegido: `c59b24017f5daebb4e37512d799a0d07f589bce5`; diff de produccion/Makefile vacio.
- Unitarios ADS1263: `66/66 PASS`. El primer intento fallo en link por alcance accidental del wrapper stdio; se preservo como fallo de infraestructura y el rerun limpio paso.
- Unitarios acquisition: `30/30 PASS`: seis casos de robustez existentes y los 24 casos exactos `CFT-P01..P24`; todos los CFT estan `PASS`.
- Integracion host: dos runs independientes con el mismo inventario de 71 IDs; cada run `71 PASS`, `0 FAIL`, `0 BLOCKED`. Es evidencia software-only con hardware simulado o sustituido.
- Analisis estatico: Cppcheck no-gpiod completo con 41 hallazgos style, 32 `missingIncludeSystem` y metadata/limites; 38 falsos positivos de contexto/herramienta, tres observaciones aceptadas de interfaz publica y cero defectos confirmados. La configuracion libgpiod-v2 esta `BLOCKED` por falta de `pkg-config` y `gpiod.h`; GCC fallback no-gpiod tuvo cero diagnosticos.
- Cobertura ADS1263: 151/222 lineas, 13/16 funciones y 90/134 branches, sin cambio. Acquisition: 118/168 lineas, 9/13 funciones y 53/76 branches, frente a 54/168, 6/13 y 26/76. El intento gcovr acquisition paralelo fallo por carrera sobre el arbol compartido y el rerun serial paso; ambos se preservan.
- Defectos confirmados por TSSE: DEF-001 permanece abierto y sin correccion de produccion. Los estados ROB y las ambiguedades contractuales se conservan.
- Hardware utilizado por TSSE: ninguno. No se verifican target, SPI/GPIO fisicos, exactitud ADC ni tiempo real.
- Cierre humano: aprobacion final del reporte y ensayo/rehearsal `BLOCKED / PENDING HUMAN`; no se infiere cierre academico.
