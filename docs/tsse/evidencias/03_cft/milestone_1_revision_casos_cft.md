# Revision independiente de casos CFT - Milestone 1

## Alcance

- Actividad: revision de diseno; no es ejecucion de pruebas.
- Casos: `docs/tsse/cft_cases.md`.
- Modelo: `docs/tsse/cft_acquisition_run_samples.md` y su DOT.
- Fuente: `components/acquisition/src/acquisition.c` del baseline `c59b24017f5daebb4e37512d799a0d07f589bce5`.

## Metodo

Un revisor independiente verifico las 24 secuencias contra el CFG y la fuente, incluyendo factibilidad, limites, scripts externos, retornos, eventos, cuentas, archivo, cleanup, estados y limites de afirmacion.

## Hallazgos y resolucion

| ID | Hallazgo | Resolucion | Estado |
|---|---|---|---|
| CASE-R01 | Los paths N=2 usaban rate 1, que produce `tv_nsec=1000000000` invalido para POSIX | Se registro DSP-017; N=2 usa rate 2 y rate 1 se cubre con N=1 sin sleep | CORREGIDO |
| CASE-R02 | Abreviaciones de secuencia no permitian expansion exacta | Se definieron prefijos contiguos y todas las secuencias quedaron expandibles sin elipsis | CORREGIDO |
| CASE-R03 | P20/P24 eran el mismo path estructural | P24 ahora abre CSV, escribe header, falla en read y cierra CSV antes del ADC | CORREGIDO |
| CASE-R04 | Faltaban razones de factibilidad y fixtures concretos | Se agregaron razones por path, ADC/config/snapshot/sample/status y eventos ordenados | CORREGIDO |
| CASE-R05 | La fila CSV podia atribuir formato interno de logger a acquisition | El oracle limita acquisition a lifecycle/header y llamada al contrato logger | CORREGIDO |
| CASE-R06 | Algunos enlaces REQ/ROB excedian la frontera o no fijaban SPI_TRANSFER | Se calificaron enlaces parciales/contextuales y ROB-002 usa `SPI_TRANSFER` explicito | CORREGIDO |
| CASE-R07 | P22 no fijaba el texto externo del status | Se agrego `status text=SPI_TRANSFER` al script | CORREGIDO |

## Disposicion

`APROBADO`. Los 24 paths son estructuralmente distintos y factibles bajo sus scripts declarados; cada uno tiene un caso `NOT RUN`. Los limites requeridos estan mapeados y N=1/N=2 tienen respectivamente cero/una espera logica. La aprobacion es de diseno y no afirma ejecucion, internals ADS1263/HAL, hardware, timing real, persistencia fisica ni cobertura exhaustiva.
