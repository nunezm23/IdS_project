# Catalogo atomico de casos - TSSE

## Convenciones

Este catalogo es el indice canonico de casos planificados y analiticos. Los casos CFT delegan su script y oracle completos a `docs/tsse/cft_cases.md`, que forma parte del registro atomico. No se infiere ejecucion por estar catalogado.

Campos obligatorios: ID, fuente, nivel, tecnica, precondiciones, inputs/script, oracle, estado, evidencia y defecto. Estados permitidos: `PASS`, `FAIL`, `BLOCKED`, `NOT RUN`, `OUT OF SCOPE`.

- `HOST`: Ubuntu x86-64 preparado, fuente de produccion preservada y doubles sin hardware.
- `CORE`: `components/ads1263/src/ads1263.c` real con SPI/GPIO/esperas controlados.
- `ACQ`: `components/acquisition/src/acquisition.c` real con ADS1263/logger/archivo/tiempo controlados.
- `PROC`: proceso host con modulos reales declarados y hardware sustituido.
- Evidencia `pendiente:<ruta>` identifica un destino futuro, no un artefacto existente.
- Defecto `N/A` significa sin defecto confirmado; un ROB sigue siendo hipotesis hasta evidencia y contrato.

## Auditoria, analisis y alcance

| ID | Fuente | Nivel / tecnica | Precondiciones | Inputs o script | Oracle | Estado | Evidencia | Defecto |
|---|---|---|---|---|---|---|---|---|
| AUD-BASE-001 | TST-001 | Auditoria / inspeccion | Repo accesible | rama, HEAD, baseline, host, checksums | Metadatos y hashes completos; produccion sin diff | `PASS` | `docs/tsse/evidencias/01_proyecto/milestone_1_linea_base.md` | N/A |
| AUD-PROD-001 | TST-010 | Gate / diff Git | Baseline c59b240 | guarda C/H y Makefile | Diff vacio; ninguna correccion autorizada | `PASS` | `docs/tsse/evidencias/01_proyecto/milestone_1_guarda_produccion.md` | N/A |
| REV-MTP-001 | TST-002 | Revision / checklist | Master Test Plan completo | alcance, niveles, criterios, riesgos, evidencia | Disposicion aprobada y supuestos visibles | `PASS` | `docs/tsse/evidencias/01_proyecto/milestone_1_revision_plan.md` | N/A |
| REV-CFT-001 | TST-003 | Revision / source-to-model | Fuente y CFG preservados | nodos, edges, decisiones, lineas, complejidad | Modelo corresponde a fuente; hallazgos corregidos | `PASS` | `docs/tsse/evidencias/03_cft/milestone_1_revision_cft.md` | N/A |
| REV-CFT-002 | TST-002, TST-003 | Revision / path-to-case | Modelo y 24 casos completos | paths, limites, scripts, oraculos, claims | Mapeo aprobado; cada caso sigue `NOT RUN` | `PASS` | `docs/tsse/evidencias/03_cft/milestone_1_revision_casos_cft.md` | N/A |
| ARCH-001 | REQ-012 | Analisis / inspeccion | C/H preservados | ubicacion y tipos Linux expuestos | Ubicacion observable; criterio de abstraccion requiere decision | `BLOCKED` | `docs/tsse/discrepancias.md#dsp-003` | N/A |
| HW-LEGACY-001 | REQ-001 | Sistema / aceptacion hardware | STM32MP157D-DK1 y ADS1263 fisicos | build/deploy/ejecucion target legacy | No se ejecuta en alcance host TSSE | `OUT OF SCOPE` | `docs/acceptance_tests.md` | N/A |
| AUD-SOURCE-001 | TST-004 | Auditoria / build inputs | Ceedling configurado | fuentes enlazadas por suite | Cada SUT resuelve a `.c` de produccion, sin copia | `PASS` | `docs/tsse/evidencias/01_proyecto/milestone_3a_20261015T150616Z/README.md` | N/A |
| RUN-DET-001 | TST-005 | Unit / repeticion | Suites limpias sin devices | dos ejecuciones completas por proyecto | Inventario y outcomes equivalentes; sin waits/devices reales | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_3a_20261015T150616Z/README.md` | N/A |
| ANA-COV-001 | TST-007 | Analisis / gcov-gcovr | Suites identificadas completas | perfiles coverage y filtros produccion | Metricas line/branch/function por modulo, sin umbral inventado | `PASS` | `docs/tsse/evidencias/06_cobertura/milestone_3a_20261015T150616Z/README.md` | N/A |
| ANA-STATIC-001 | TST-008 | Analisis / Cppcheck | Tool/version verificado | `ADS1263_NO_GPIOD=1`, scope C/H | Raw output, exit y findings completos | `NOT RUN` | `pendiente:docs/tsse/evidencias/02_analisis_estatico/ANA-STATIC-001/` | N/A |
| ANA-STATIC-002 | TST-008 | Analisis / Cppcheck | Headers libgpiod v2 disponibles | rama GPIO real, scope C/H | Raw output o `BLOCKED` explicito; nunca “clean” incompleto | `NOT RUN` | `pendiente:docs/tsse/evidencias/02_analisis_estatico/ANA-STATIC-002/` | N/A |
| AUD-EVID-001 | TST-009 | Auditoria / manifest | Runs futuros preservados | metadata, raw links, estados | Cada claim resuelve a evidencia o no-ejecucion explicita | `NOT RUN` | `pendiente:docs/tsse/evidencias/08_trazabilidad/AUD-EVID-001/` | N/A |

## Casos CFT

Para cada fila: nivel `Componente`; tecnica `CFT + particion/limite cuando aplica`; precondiciones, input, script, accion `RUN`, retorno, logs, outputs, cuentas, archivo y cleanup estan completos en la fila homonima de `docs/tsse/cft_cases.md`.

| ID | Fuente | Precondiciones / inputs / oracle | Estado | Evidencia | Defecto |
|---|---|---|---|---|---|
| CFT-P01 | REQ-011, TST-003 | Registro canonico CFT-P01: adc nulo, short-circuit, retorno -1 | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P01/` | N/A |
| CFT-P02 | REQ-011, TST-003 | Registro canonico CFT-P02: config nula, log y retorno -1 | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P02/` | N/A |
| CFT-P03 | REQ-011, TST-003 | Registro canonico CFT-P03: samples 0 rechazado | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P03/` | N/A |
| CFT-P04 | REQ-011, TST-003 | Registro canonico CFT-P04: samples 1001 rechazado | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P04/` | N/A |
| CFT-P05 | REQ-011, TST-003 | Registro canonico CFT-P05: rate 0 rechazado | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P05/` | N/A |
| CFT-P06 | REQ-011, TST-003 | Registro canonico CFT-P06: rate 11 rechazado | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P06/` | N/A |
| CFT-P07 | REQ-007, REQ-011, TST-003 | Registro canonico CFT-P07: canal no soportado rechazado | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P07/` | N/A |
| CFT-P08 | REQ-011, TST-003 | Registro canonico CFT-P08: tres errores acumulados | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P08/` | N/A |
| CFT-P09 | TST-003 | Registro canonico CFT-P09: init SPI_OPEN | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P09/` | N/A |
| CFT-P10 | TST-003 | Registro canonico CFT-P10: init GPIO_OPEN | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P10/` | N/A |
| CFT-P11 | TST-003 | Registro canonico CFT-P11: init SPI_CONFIG | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P11/` | N/A |
| CFT-P12 | REQ-005 parcial, TST-003 | Registro canonico CFT-P12: presence ZERO y cleanup | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P12/` | N/A |
| CFT-P13 | REQ-005 parcial, TST-003 | Registro canonico CFT-P13: presence FLOATING y cleanup | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P13/` | N/A |
| CFT-P14 | REQ-004 parcial, TST-003 | Registro canonico CFT-P14: mismatch, snapshot y cleanup | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P14/` | N/A |
| CFT-P15 | REQ-004 parcial, TST-003 | Registro canonico CFT-P15: presence IO y cleanup | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P15/` | N/A |
| CFT-P16 | REQ-007 parcial, TST-003 | Registro canonico CFT-P16: configure SPI_TRANSFER y cleanup | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P16/` | N/A |
| CFT-P17 | REQ-007 parcial, REQ-008, TST-003 | Registro canonico CFT-P17: N=1/rate=1, sin sleep, retorno 0 | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P17/` | N/A |
| CFT-P18 | REQ-008, REQ-010 caracterizacion, TST-003 | Registro canonico CFT-P18: open CSV falla, adquisicion continua | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P18/` | N/A |
| CFT-P19 | REQ-008, REQ-010, TST-003 | Registro canonico CFT-P19: CSV/header/output/cierre | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P19/` | N/A |
| CFT-P20 | REQ-008, ROB-002, TST-003 | Registro canonico CFT-P20: primera lectura SPI_TRANSFER | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P20/` | N/A |
| CFT-P21 | REQ-007 parcial, REQ-008, TST-003 | Registro canonico CFT-P21: N=2/rate=2, back edge y un sleep | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P21/` | N/A |
| CFT-P22 | REQ-008, ROB-002, TST-003 | Registro canonico CFT-P22: segunda lectura SPI_TRANSFER | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P22/` | N/A |
| CFT-P23 | REQ-008, TST-003 | Registro canonico CFT-P23: EINTR y retry finito | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P23/` | N/A |
| CFT-P24 | REQ-008, REQ-010 parcial, ROB-002, TST-003 | Registro canonico CFT-P24: N=1000/rate=10, CSV y fallo temprano | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/CFT-P24/` | N/A |

## Unitarios ADS1263 core y adapters

Todos requieren `HOST`; el oracle se evalua contra la traduccion de produccion real. Evidencia prevista: `docs/tsse/evidencias/04_unitarias/<ID>/`.

| ID | Fuente | Nivel / tecnica | Precondiciones | Inputs o script | Oracle | Estado | Evidencia | Defecto |
|---|---|---|---|---|---|---|---|---|
| UT-VOLT-001 | ROB-004 | Unit / particion | CORE | raw=0, gain=1, vref=2.5 | voltaje 0; tolerancia absoluta 1e-12 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2a_20261015T142406Z/README.md` | N/A |
| UT-VOLT-002-A | ROB-004 | Unit / equivalencia | CORE | raw=1073741824,vref=2.5,gain=1 | `(raw/2147483647.0)*2.5`, positivo, tolerancia 1e-12 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2a_20261015T142406Z/README.md` | N/A |
| UT-VOLT-002-B | ROB-004 | Unit / equivalencia | CORE | raw=-1073741824,vref=2.5,gain=1 | `(raw/2147483647.0)*2.5`, negativo, tolerancia 1e-12 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2a_20261015T142406Z/README.md` | N/A |
| UT-VOLT-003-A | ROB-004 | Unit / limite | CORE | raw=INT32_MAX,vref=2.5,gain=1 | 2.5 finito, tolerancia 1e-12 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2a_20261015T142406Z/README.md` | N/A |
| UT-VOLT-003-B | ROB-004 | Unit / limite | CORE | raw=INT32_MIN,vref=2.5,gain=1 | `(INT32_MIN/2147483647.0)*2.5`, finito, tolerancia 1e-12 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2a_20261015T142406Z/README.md` | N/A |
| UT-VOLT-004-A | ROB-004 | Unit / parametros | CORE | raw=1073741824,vref=2.5,gain=2 | `(raw/2147483647.0)*1.25`, tolerancia 1e-12 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2a_20261015T142406Z/README.md` | N/A |
| UT-VOLT-004-B | ROB-004 | Unit / parametros | CORE | raw=1073741824,vref=5.0,gain=1 | `(raw/2147483647.0)*5.0`, tolerancia 1e-12 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2a_20261015T142406Z/README.md` | N/A |
| UT-VOLT-005 | ROB-004 | Unit / caracterizacion | CORE | raw=1073741824,gain=0,vref=2.5 | Mismo resultado que gain=1 actual; tolerancia 1e-12; sin asumir contrato | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2a_20261015T142406Z/README.md` | N/A |
| UT-REG-001-A | REQ-004 | Unit / argumento | CORE | read dev=NULL,out valido,start=0,count=1 | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-001-B | REQ-004 | Unit / argumento | CORE | read dev valido,out=NULL,start=0,count=1 | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-001-C | REQ-004 | Unit / argumento | CORE | write dev=NULL,values={0xAA},start=0,count=1 | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-001-D | REQ-004 | Unit / argumento | CORE | write dev valido,values=NULL,start=0,count=1 | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-002 | REQ-004 | Unit / limite | CORE | read start=0,count=1; RX[2]=0xA5 | TX `{0x20,0x00,0x00}`, len=3; out=0xA5; OK | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-003 | REQ-004 | Unit / limite | CORE | read start=31,count=1; RX[2]=0x5A | TX `{0x3F,0x00,0x00}`, len=3; out=0x5A; OK | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-004-A | REQ-004 | Unit / limite read | CORE | read start=0,count=0 | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-004-B | REQ-004 | Unit / limite read | CORE | read start=0,count=32; RX[2..33]=0..31 | TX command=0x20,count=0x1F,len=34; out=0..31; OK | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-004-C | REQ-004 | Unit / limite read | CORE | read start=0,count=33 | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-004-D | REQ-004 | Unit / limite read | CORE | read start=32,count=1 | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-005 | REQ-004, ROB-002 | Unit / error | CORE | read start=0,count=1; out=0xCC; transfer=SPI_TRANSFER | SPI_TRANSFER; out permanece 0xCC; una transfer | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-006-A | REQ-004 | Unit / frame write | CORE | write start=0,count=1,value=0xAA | TX `{0x40,0x00,0xAA}`,len=3; OK | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-006-B | REQ-004 | Unit / frame write | CORE | write start=31,count=1,value=0x55 | TX `{0x5F,0x00,0x55}`,len=3; OK | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-007 | ROB-001 | Unit+ASan+UBSan / caracterizacion | CORE aislado | read start=31,count=2 con canarios | OK; lee registro 31 y byte adyacente; canarios externos intactos; sanitizers sin diagnostico | `PASS` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | N/A; `FND-ROB001-001` sospechado |
| UT-REG-008 | ROB-001 | Unit+ASan+UBSan / caracterizacion | CORE aislado | write start=31,count=2,values={0x12,0x34}, canarios 0xA5 | OK; cambia registro 31 y byte adyacente; resto/canarios intactos; sanitizers sin diagnostico | `PASS` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | N/A; `FND-ROB001-001` sospechado |
| UT-REG-009-A | REQ-004 | Unit / limite write | CORE | write start=0,count=0,values valido | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-009-B | REQ-004 | Unit / limite write | CORE | write start=0,count=32,values=0..31 | TX command=0x40,count=0x1F,payload 0..31,len=34; OK | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-009-C | REQ-004 | Unit / limite write | CORE | write start=0,count=33,values valido | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-009-D | REQ-004 | Unit / limite write | CORE | write start=32,count=1,value=0xAA | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-010 | REQ-004, ROB-002 | Unit / error write | CORE | write start=0,count=1,value=0xAA; transfer=SPI_TRANSFER | TX `{0x40,0x00,0xAA}`,len=3; SPI_TRANSFER | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-011-A | REQ-004 | Unit / nominal mock | CORE mock | read start=5,count=2 con registros `{0x6C,0xA3}` | out exacto `{0x6C,0xA3}`; OK; cero transfers SPI | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-REG-011-B | REQ-004 | Unit / nominal mock | CORE mock | write start=5,count=2,values=`{0x12,0x34}` | `mock_registers[5..6]` exactos; OK; cero transfers SPI | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-001 | REQ-004 | Unit / argumento | CORE | dev nulo; snapshot valido | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-002 | REQ-004 | Unit / argumento | CORE | dev valido; snapshot nulo | INVALID_ARG; cero transfers | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-003 | REQ-004, ROB-002 | Unit / error | CORE | RREG=SPI_TRANSFER | SPI_TRANSFER; snapshot sin afirmacion de contenido | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-004 | REQ-005 | Unit / equivalencia | CORE | registros `{00,00,00,00,00}` | NOT_DETECTED_ZERO y snapshot exacto | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-005 | REQ-005 | Unit / equivalencia | CORE | registros `{FF,FF,FF,FF,FF}` | NOT_DETECTED_FLOATING y snapshot exacto | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-006 | REQ-004 | Unit / decision | CORE | `{23,10,05,00,80}` POWER distinto | REGISTER_MISMATCH; snapshot exacto | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-007 | REQ-004 | Unit / decision | CORE | `{23,11,04,00,80}` INTERFACE distinto | REGISTER_MISMATCH; snapshot exacto | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-008 | REQ-004 | Unit / decision | CORE | `{23,11,05,01,80}` MODE0 distinto | REGISTER_MISMATCH; snapshot exacto | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-009 | REQ-004 | Unit / decision | CORE | `{23,11,05,00,81}` MODE1 distinto | REGISTER_MISMATCH; snapshot exacto | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-010 | REQ-004 | Unit / equivalencia | CORE | `{42,11,05,00,80}` ID alternativo | OK; snapshot exacto; ID no causa rechazo | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-PRES-011 | REQ-004 | Unit / nominal | CORE | `{23,11,05,00,80}` | OK y snapshot exacto | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-CHAN-001 | REQ-007 | Unit / argumento | CORE | dev nulo, channel=0 | INVALID_ARG; cero calls | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-CHAN-002 | REQ-007 | Unit / equivalencia | CORE | dev valido, channel=1 | INVALID_ARG; cero transfer | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-CHAN-003 | REQ-007, ROB-002 | Unit / error | CORE | channel=0; write INPMUX=SPI_TRANSFER | SPI_TRANSFER; read/START1 suprimidos | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-CHAN-004 | REQ-007, ROB-002 | Unit / error | CORE | write OK, read=SPI_TRANSFER | SPI_TRANSFER; START1 suprimido | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-CHAN-005 | REQ-007 | Unit / decision | CORE | write OK; readback=0xFF | REGISTER_MISMATCH; START1 suprimido | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-CHAN-006 | REQ-007, ROB-002 | Unit / error | CORE | write/read OK; START1=SPI_TRANSFER | SPI_TRANSFER despues de write/read | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-CHAN-007 | REQ-007 | Unit / nominal | CORE | channel=0 y transfers OK | orden WREG,RREG,START1; OK | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-SAMPLE-001-A | REQ-008 | Unit / argumento | CORE | dev=NULL,sample valido | INVALID_ARG; cero HAL calls | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-SAMPLE-001-B | REQ-008 | Unit / argumento | CORE | dev valido,sample=NULL | INVALID_ARG; cero HAL calls | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-SAMPLE-002 | REQ-008 | Unit / nominal mock | CORE mock, vref=2.5,gain=1,counter=0 | dos lecturas | raw 1234567 y 2469134; status OK; contador=2; voltajes por formula | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-SAMPLE-003 | REQ-008 | Unit / frame | CORE no-mock,use_drdy=0 | RX `{00,80,00,00,00,01,5A}` | TX `{12,00,00,00,00,00,00}`,len=7; raw=1,status=0x80,checksum=0x5A,drdy=-1,text OK | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-SAMPLE-004 | ROB-004 | Unit / limite | CORE | RX `{00,80,00,00,00,00,5A}` | raw=0,voltage=0,status=0x80,checksum=0x5A | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-SAMPLE-005 | ROB-004 | Unit / limite | CORE | RX raw bytes `{7F,FF,FF,FF}`, vref=2.5,gain=1 | raw=INT32_MAX; voltage=2.5 dentro 1e-12 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-SAMPLE-006 | ROB-004 | Unit / limite | CORE | RX raw bytes `{80,00,00,00}`, vref=2.5,gain=1 | raw=INT32_MIN; formula signed dentro 1e-12 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-SAMPLE-007 | ROB-004 | Unit / campos | CORE,use_drdy=0,vref=2.5,gain=1 | RX `{00,A5,00,00,00,2A,3C}` | raw=42,status=0xA5,checksum=0x3C,drdy=-1,text OK,voltaje por formula 1e-12 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-SAMPLE-008 | ROB-002 | Unit / error | CORE, use_drdy=0; sample prefill 0xA5 | SPI RDATA1=SPI_TRANSFER | SPI_TRANSFER; sample queda zeroed, `status=NULL`; una transfer | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-DRDY-001-A | REQ-006 | Unit / argumento | CORE | `ads1263_read_drdy(NULL,&value)` | INVALID_ARG; cero GPIO calls | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-DRDY-001-B | REQ-006 | Unit / argumento | CORE | `ads1263_read_drdy(dev,NULL)` | INVALID_ARG; cero GPIO calls | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-DRDY-002 | REQ-006 | Unit / nominal | CORE, wait interpuesto | GPIO retorna ready=0 inmediato | un poll, cero waits; SPI posterior una vez | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-DRDY-003 | REQ-006 | Unit / loop acotado | CORE, wait interpuesto | GPIO=1,1,0 | tres polls/dos waits; luego SPI una vez | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-DRDY-004 | REQ-006, ROB-003 | Unit / caracterizacion | CORE,vref=2.5,gain=1 | GPIO_READ sin modificar drdy=-1; SPI success RX `{00,A5,00,00,00,2A,3C}` | return OK; SPI=1; raw=42,voltage formula 1e-12,drdy=-1,status_byte=0xA5,checksum=0x3C,status=`GPIO_READ` | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-DRDY-005 | REQ-006, ROB-003 | Unit / caracterizacion | CORE,waits interpuestos,vref=2.5,gain=1 | 1000 GPIO busy=1; SPI success RX `{00,A5,00,00,00,2A,3C}` | return OK; 1000 polls/waits; SPI=1; raw=42,voltage formula 1e-12,drdy=1,status_byte=0xA5,checksum=0x3C,status=`DRDY_TIMEOUT` | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2b_20261015T144924Z/README.md` | N/A |
| UT-DRDY-006 | REQ-006, ROB-003 | Unit / caracterizacion | CORE | GPIO_READ; SPI=SPI_TRANSFER | return SPI_TRANSFER; SPI=1; sample queda zeroed | `PASS` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | N/A |
| UT-DRDY-007 | REQ-006, ROB-003 | Unit / caracterizacion | CORE, waits interpuestos | 1000 busy; SPI=SPI_TRANSFER | return SPI_TRANSFER; 1000 polls/waits; SPI=1; sample queda zeroed | `PASS` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | N/A |
| UT-ACQ-ROB003-001 | ROB-003 | Unit acquisition / consecuencia | ACQ,N=1,rate=2 | read=OK con sample.status=`GPIO_READ`,drdy=-1 | return 0; una salida con GPIO_READ; read=1,sleep=0,close=1 | `PASS` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | N/A |
| UT-ACQ-ROB003-002 | ROB-003 | Unit acquisition / consecuencia | ACQ,N=1,rate=2 | read=OK con sample.status=`DRDY_TIMEOUT`,drdy=1 | return 0; una salida con DRDY_TIMEOUT; read=1,sleep=0,close=1 | `PASS` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | N/A |
| UT-ACQ-ROB003-003 | ROB-003 | Unit acquisition / consecuencia | ACQ,N=1,rate=2; evidencia pareada GPIO_READ+SPI fail | read=SPI_TRANSFER | return -1; cero outputs; error SPI_TRANSFER; close=1 | `PASS` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | N/A |
| UT-ACQ-ROB003-004 | ROB-003 | Unit acquisition / consecuencia | ACQ,N=1,rate=2; evidencia pareada timeout+SPI fail | read=SPI_TRANSFER | return -1; cero outputs; error SPI_TRANSFER; close=1 | `PASS` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | N/A |
| UT-ACQ-DSP017-001 | REQ-008, DSP-017 | Unit acquisition / caracterizacion | ACQ,N=2,rate=1; wait interpuesto | dos EINVAL y luego success de barrera | tres calls con `{0,1000000000}`; run acotado retorna 0 | `PASS` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | DEF-001 |
| UT-ACQ-DSP017-002 | REQ-008, DSP-017 | Unit host / POSIX | HOST con nanosleep real | timespec `{0,1000000000}` | nanosleep host=-1, errno=EINVAL | `PASS` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` | DEF-001 |
| PROC-DSP017-001 | REQ-008, DSP-017 | Proceso / reproduccion acotada | acquisition real; ADS/logger fakes | N=2,rate=1; timeout externo 2 s | return 0 y dos outputs dentro del limite | `FAIL` | `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/03_dsp017_probe_timeout.typescript` | DEF-001 |
| UT-CFG-001 | REQ-003, REQ-006 | Unit / nominal | CORE | `ads1263_default_config()` | `/dev/spidev0.0`,`/dev/gpiochip6`,8,1000000,2.5,1,use_drdy=1,mock=0 | `PASS` | `docs/tsse/evidencias/04_unitarias/milestone_2a_20261015T142406Z/README.md` | N/A |
| UT-CFG-002 | REQ-003 | Unit / particion | CORE | spi=`/tmp/fake`,chip=NULL,line=17,speed=500000,vref=3.3,gain=2,use_drdy=0,mock=1 | init OK; config copiada; initialized=1; mock baseline; HAL=0 | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/UT-CFG-002/` | N/A |
| UT-SPI-001-A | REQ-002 | Unit adapter / argumento | HAL fake | open spi=NULL,device=`/dev/fake`,speed=1000000 | INVALID_ARG; open/ioctl=0 | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/UT-SPI-001-A/` | N/A |
| UT-SPI-001-B | REQ-002 | Unit adapter / argumento | HAL fake | open spi valido,device=NULL,speed=1000000 | INVALID_ARG; open/ioctl=0 | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/UT-SPI-001-B/` | N/A |
| UT-SPI-002 | REQ-002 | Unit adapter / path | HAL fake | device=`/dev/fake-spi`, speed=1000000; open=-1 | path/flags exactos; SPI_OPEN; ioctl=0 | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/UT-SPI-002/` | N/A |
| UT-SPI-003 | REQ-002 | Unit adapter / nominal | HAL fake | open fd=7; cuatro ioctls success | fd=7,mode=1,bits=8,speed=1000000; orden exacto; OK | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/UT-SPI-003/` | N/A |
| UT-SPI-004 | REQ-002, ROB-002 | Unit adapter / error | HAL fake | fd=7,tx={0x12,0},rx=0xA5,len=2; ioctl=-1 | SPI_TRANSFER; una ioctl; RX permanece 0xA5 | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/UT-SPI-004/` | N/A |
| UT-SPI-005 | REQ-003 | Unit adapter / frame config | HAL fake | open fd=7,speed=1000000; ioctls OK | WR_MODE=1,WR_BITS=8,WR_SPEED=1000000,WR_LSB_FIRST=0; sin waveform claim | `NOT RUN` | `pendiente:docs/tsse/evidencias/04_unitarias/UT-SPI-005/` | N/A |

## Integracion host y CLI

Todos usan `PROC`; evidencia prevista: `docs/tsse/evidencias/05_integracion_host/<ID>/`.

| ID | Fuente | Nivel / tecnica | Precondiciones | Inputs o script | Oracle | Estado | Evidencia | Defecto |
|---|---|---|---|---|---|---|---|---|
| INT-MOCK-001 | REQ-002, TST-006 | Integracion / proceso | Binario host mock | `ads1263_app --check --mock` | exit 0, baseline simulado, sin devices | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/INT-MOCK-001/` | N/A |
| INT-ACQ-001 | REQ-008, TST-006 | Integracion / proceso | Binario host mock | `--samples 1 --channel 0 --rate-hz 2 --mock` | una muestra indice 1, exit 0, cleanup | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/INT-ACQ-001/` | N/A |
| INT-ACQ-002 | REQ-008, TST-006 | Integracion / proceso | Binario host mock | `--samples 2 --channel 0 --rate-hz 2 --mock` | dos muestras indices 1/2, exit 0 | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/INT-ACQ-002/` | N/A |
| INT-ACQ-003 | REQ-008, REQ-011, TST-006 | Integracion / limite | Binario host mock | `--samples 0 --channel 0 --rate-hz 2 --mock` | exit no-cero; `invalid samples value. Valid range: 1..1000` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/INT-ACQ-003/` | N/A |
| INT-ACQ-004 | REQ-008, TST-006 | Integracion / caracterizacion condicional | Gate scripted HAL no aprobado | Primera sample retorna SPI_TRANSFER | Preservar exit/diagnostico/cleanup observado; no oracle de conformance antes del gate | `BLOCKED` | `pendiente:docs/tsse/evidencias/05_integracion_host/INT-ACQ-004/`; bloqueado por gate 11.6 | N/A |
| INT-CSV-001 | REQ-010, TST-006 | Integracion / archivo | Temp file con contenido `OLD` | `--samples 2 --channel 0 --rate-hz 2 --mock --csv <temp>` | `OLD` truncado; header exacto; dos filas; cierre; exit 0 | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/INT-CSV-001/` | N/A |
| INT-CSV-002 | REQ-010, TST-006 | Integracion / contenido | Logger real host, counter mock inicial 0 | `--samples 1 --channel 0 --rate-hz 2 --mock --csv <temp>` | fila exacta `1,1234567,0.001437225,OK`; exit 0 | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/INT-CSV-002/` | N/A |
| INT-CSV-003 | REQ-010, TST-006 | Integracion / caracterizacion | Directorio padre inexistente | fixed mock N=1 + csv invalido | dos warnings, una muestra de consola y exit 0 actual; DSP-006 | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/INT-CSV-003/` | N/A |
| INT-LIVE-001 | REQ-009, TST-006 | Integracion / proceso acotado | Control de child fiable, timeout 5 s | `--live --channel 0 --rate-hz 2 --mock`; SIGINT tras primera muestra | termina antes de 5 s, cleanup, exit observado, ningun child | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/INT-LIVE-001/` | N/A |
| INT-LIVE-002 | REQ-009, TST-006 | Integracion / repeticion | Igual anterior, dos runs | SIGINT despues de al menos 2 muestras en cada run | ambos terminan antes de 5 s y dejan cero children; inventario comparable | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/INT-LIVE-002/` | N/A |
| CLI-001 | REQ-011, TST-006 | Integracion / gramatica | Binario host | sin argumentos | EXIT_FAILURE y usage | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-001/` | N/A |
| CLI-002 | REQ-011, TST-006 | Integracion / gramatica | Binario host | `--help` | usage y EXIT_SUCCESS | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-002/` | N/A |
| CLI-003-A | REQ-011, TST-006 | Integracion / limite | Binario host | `--samples 0 --mock` | EXIT_FAILURE; `invalid samples value. Valid range: 1..1000` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-003-A/` | N/A |
| CLI-003-B | REQ-011, TST-006 | Integracion / limite | Binario host | `--samples 1001 --mock` | EXIT_FAILURE; `invalid samples value. Valid range: 1..1000` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-003-B/` | N/A |
| CLI-003-C | REQ-011, TST-006 | Integracion / limite | Binario host | `--samples 1 --rate-hz 0 --mock` | EXIT_FAILURE; `invalid rate-hz value. Valid range: 1..10` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-003-C/` | N/A |
| CLI-003-D | REQ-011, TST-006 | Integracion / limite | Binario host | `--samples 1 --rate-hz 11 --mock` | EXIT_FAILURE; `invalid rate-hz value. Valid range: 1..10` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-003-D/` | N/A |
| CLI-003-E | REQ-011, TST-006 | Integracion / limite | Binario host | `--samples 1 --channel 1 --mock` | EXIT_FAILURE; `invalid channel value. Valid range: 0..0` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-003-E/` | N/A |
| CLI-003-F | REQ-011, TST-006 | Integracion / limite | Binario host | `--check --drdy-line 1024 --mock` | EXIT_FAILURE; `invalid drdy-line value. Valid range: 0..1023` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-003-F/` | N/A |
| CLI-004-A | REQ-011, TST-006 | Integracion / sintaxis | Binario host | `--samples` | EXIT_FAILURE; `--samples requires a value` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-004-A/` | N/A |
| CLI-004-B | REQ-011, TST-006 | Integracion / sintaxis | Binario host | `--samples abc --mock` | EXIT_FAILURE; `invalid samples value. Valid range: 1..1000` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-004-B/` | N/A |
| CLI-004-C | REQ-011, TST-006 | Integracion / sintaxis | Binario host | `--check --spi-dev` | EXIT_FAILURE; `--spi-dev requires a value` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-004-C/` | N/A |
| CLI-004-D | REQ-011, TST-006 | Integracion / sintaxis | Binario host | `--check --drdy-line abc --mock` | EXIT_FAILURE; `invalid drdy-line value. Valid range: 0..1023` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-004-D/` | N/A |
| CLI-005 | REQ-011, TST-006 | Integracion / equivalencia | Binario host | `--unknown` | EXIT_FAILURE y `unknown option: --unknown` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-005/` | N/A |
| CLI-006-A | REQ-011, TST-006 | Integracion / modos | Binario host | `--check --samples 1 --mock` | EXIT_FAILURE; `select exactly one command mode` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-006-A/` | N/A |
| CLI-006-B | REQ-011, TST-006 | Integracion / modos | Binario host | `--samples 1 --check --mock` | EXIT_FAILURE; `select exactly one command mode` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-006-B/` | N/A |
| CLI-006-C | REQ-011, TST-006 | Integracion / caracterizacion | Binario host | `--check --help` | EXIT_SUCCESS y usage; bypass por ultimo modo HELP; DSP-007 | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-006-C/` | N/A |
| CLI-006-D | REQ-011, TST-006 | Integracion / caracterizacion | Binario host | `--help --check --mock` | EXIT_FAILURE; `select exactly one command mode`; DSP-007 | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-006-D/` | N/A |
| CLI-006-E | REQ-011, TST-006 | Integracion / caracterizacion | Binario host | `--check --csv out.csv --mock` | EXIT_SUCCESS actual; CSV ignorado; DSP-008 | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-006-E/` | N/A |
| CLI-006-F | REQ-011, TST-006 | Integracion / politica CSV | Binario host | `--live --channel 0 --rate-hz 2 --csv out.csv --mock` | EXIT_FAILURE; `--csv is supported with --samples mode only` | `NOT RUN` | `pendiente:docs/tsse/evidencias/05_integracion_host/CLI-006-F/` | N/A |

## Reglas de actualizacion

- Un caso cambia de `NOT RUN` solo con raw evidence identificada.
- `BLOCKED` requiere motivo y condicion de desbloqueo; `OUT OF SCOPE` requiere exclusion deliberada.
- `PASS` y `FAIL` describen una actividad realmente ejecutada o una inspeccion realmente completada, no un resultado esperado.
- El campo defecto cambia de N/A solo con un ID estable y un contrato aprobado; ROB por si solo no es defecto.
- Los destinos `pendiente:docs/tsse/evidencias/...` no existen como resultado hasta que la actividad se ejecute; al ejecutar se reemplazan por el run set real sin borrar este diseno.
