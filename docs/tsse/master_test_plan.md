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
