# Registro de discrepancias y supuestos - TSSE

## Uso

Este registro no modifica los requisitos originales ni confirma defectos. Documenta diferencias observadas entre fuente, documentos y alcance TSSE que afectan el diseno o el oraculo de una prueba.

Disposiciones del registro (no son estados de ejecucion de trazabilidad):

- `ACEPTADO PARA DISENO`: existe una regla suficiente para disenar sin cambiar el producto.
- `CARACTERIZAR`: se debe observar el comportamiento antes de decidir conformidad.
- `REQUIERE DECISION`: falta un contrato humano aprobado.
- `FUERA DE ALCANCE TSSE`: se conserva como contexto, pero no se ejecuta en host.

## Discrepancias

| ID | Tema | Evidencia observada | Impacto de testing | Disposicion actual |
|---|---|---|---|---|
| DSP-001 | Alcance legacy frente a TSSE | `docs/acceptance_tests.md:3-27,46-118` incluye cross-build ARM, target, hardware y potenciometro; `master_test_plan.md:27-32` los excluye | No mezclar aceptacion historica/producto con evidencia host actual | `FUERA DE ALCANCE TSSE`; conservar los casos legacy |
| DSP-002 | Titulo “Host build” | `acceptance_tests.md:3-15` carga un SDK Yocto y espera ELF ARM | Puede confundirse con build nativo x86-64 de pruebas | `ACEPTADO PARA DISENO`; llamarlo cross-build legacy |
| DSP-003 | REQ-012 e interfaces Linux | REQ-012 ubica codigo Linux bajo `arch/linux`, pero `ads1263.h:5-17` incluye y embebe tipos Linux | Una inspeccion puede probar ubicacion, no independencia arquitectonica completa | `REQUIERE DECISION` sobre criterio final de cumplimiento |
| DSP-004 | Responsabilidad logger/CSV | `architecture.md:13` asigna formato a logger; `acquisition.c:26-37,168-185,253-255` imprime snapshot y administra archivo/header/cierre | La frontera de prueba real incluye acquisition y logger | `ACEPTADO PARA DISENO`; modelar fuente observada |
| DSP-005 | “Append” de CSV | `architecture.md:23` dice append; `acquisition.c:176` usa `fopen(..., "w")`; `INT-CSV-TRUNCATE` paso en ambos runs Milestone 4 y observo eliminacion del contenido previo | El comportamiento host actual es crear/truncar, no anexar | `CARACTERIZADO`; ambiguedad documental, no defecto |
| DSP-006 | Fallo de apertura CSV | `INT-CSV-FAIL` paso en ambos runs: dos warnings, continuidad de dos muestras, sin CSV y exit 0; CFT-P18 tambien paso | No existe contrato explicito que diga si el fallo debe ser fatal | `REQUIERE DECISION`; comportamiento no fatal caracterizado, no defecto |
| DSP-007 | Modos CLI y help | `CLI-CHECK-HELP`, `CLI-HELP-CHECK` y las permutaciones samples/live/help pasaron en ambos runs con resultados dependientes del orden declarado | HELP procesado al final puede retornar 0 antes de validar modos; HELP antes de otro modo termina en diagnostico de modos | `CARACTERIZADO`; ambiguedad de gramatica, no defecto |
| DSP-008 | CSV con check | `CLI-CHECK-CSV` y `CLI-CSV-CHECK` pasaron en ambos runs con check ejecutado, CSV ignorado y exit 0; live+CSV fue rechazado | El mensaje de politica y la aceptacion en check no estan alineados | `CARACTERIZADO`; requiere decision contractual, no defecto |
| DSP-009 | Rate de muestreo | `acquisition.c:154-166,245-250` agrega una espera despues del trabajo; no configura MODE2 | `rate_hz` es pacing aproximado, no garantia temporal end-to-end | `ACEPTADO PARA DISENO`; no afirmar rendimiento real |
| DSP-010 | ROB-001 rango de registros | `ads1263.c:102,136` valida start/count separados; UT-REG-007/008 en `evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/` observan lectura/escritura del byte posterior al arreglo | Efecto intraobjeto reproducible; falta contrato para status de rango combinado | `SUSPECTED DEFECT`; no confirmado ni corregido sin decision contractual |
| DSP-011 | ROB-003 propagacion DRDY | `ads1263.c:338-352` conserva error DRDY en `sample.status`, pero retorna OK si SPI funciona; las cuatro combinaciones y consecuencias se ejecutaron en Milestone 3A | Falta contrato que exija abortar/retornar error | `CONTRACT AMBIGUITY`; caracterizacion completa, no defecto confirmado |
| DSP-012 | Estado de inicializacion | `ads1263_t.initialized` se escribe pero las operaciones publicas no lo consultan | Precondicion del caller no documentada con precision | `CARACTERIZAR` fuera del CFT actual |
| DSP-013 | Cierre de conversion | `ADS1263_CMD_STOP1` esta definido pero `ads1263_close()` no lo envia | El cleanup software libera recursos, no demuestra estado fisico ADC | `FUERA DE ALCANCE TSSE` hasta contrato separado |
| DSP-014 | Evidencia CFT historica | `evidencias/03_cft/codigo_flujo_cft.txt` es extracto parcial/duplicado, no CFG verificado | No puede citarse como CFT completado | `ACEPTADO PARA DISENO`; preservar y crear artefactos nuevos |
| DSP-015 | Evidencia Cppcheck historica | `cppcheck_inicial.txt` contiene solo una linea de progreso y `herramientas.txt` registro indisponibilidad | No demuestra ejecucion completa ni cero findings | `CARACTERIZAR` en seccion 12; no se ejecuta en Milestone 1 |
| DSP-016 | Validacion fisica historica | El plan informa validacion previa en placa, sin raw log adjunto en este repositorio | Es contexto del proyecto, no resultado reproducible de TSSE | `ACEPTADO PARA DISENO`; atribuir como antecedente |
| DSP-017 | Pacing con `rate_hz=1` | Milestone 3A captura `{0,1000000000}`, EINVAL y timeout 124 con N=2; `evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/` | Viola REQ-008 para una entrada rate aceptada en `1..10`; fixed mode no completa | `CONFIRMED DEFECT` DEF-001; no corregir produccion sin gate |

## Decisiones que requieren aprobacion humana futura

1. Definir si REQ-012 exige solo ubicacion de implementaciones Linux o tambien ausencia de tipos Linux en la API core.
2. Definir si solicitar CSV implica que un fallo de apertura debe hacer fallar la adquisicion.
3. Definir si un fallo/timeout DRDY debe ser retorno fatal de `ads1263_read_sample()` o estado diagnostico de una muestra obtenida por SPI.
4. Aprobar, rechazar o diferir una correccion separada para DEF-001; Milestone 3A no selecciona la implementacion.

Estas decisiones no bloquean el modelo CFT: los contratos externos se parametrizan y los casos ambiguos se etiquetan como caracterizacion. Si se pretendiera modificar produccion, el flujo debe detenerse en el gate OpenSpec correspondiente.
