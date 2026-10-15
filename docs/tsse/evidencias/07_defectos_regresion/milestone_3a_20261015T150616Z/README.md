# Investigaciones controladas - Milestone 3A

## Alcance y entorno

- Run set: `milestone_3a_20261015T150616Z`.
- SUT reales: `components/ads1263/src/ads1263.c` y `components/acquisition/src/acquisition.c`.
- Host: Ubuntu 26.04.1 LTS x86-64, GCC 15.2.0, Ceedling 1.1.10, Unity/CMock 2.7.2.
- Dependencias: SPI/GPIO o ADS1263 mockeados; logger es spy; waits interpuestos salvo el probe POSIX acotado.
- Hardware y `/dev/spidev*`/`/dev/gpiochip*`: no usados.
- Metadata y checksums: `../../01_proyecto/milestone_3a_20261015T150616Z/` y `../../milestone_3a_checksums.typescript`.

## Resultados por riesgo

| Riesgo/casos | Contrato relacionado | Preconditions e inputs | Expected de investigacion | Observed | Estado/exit | Clasificacion | Evidencia y limitacion |
|---|---|---|---|---|---|---|---|
| ROB-001, UT-REG-007 | Hipotesis ROB-001; banco interno de 32 bytes | CORE mock; `start=31,count=2`; registro 31=`0x5A` | Conservar status, bytes y canarios sin asumir rechazo | Retorna OK; output=`{0x5A,byte adyacente}`; canarios del output y del contenedor intactos | `PASS`; 0 normal/ASan/UBSan | `SUSPECTED DEFECT` | `05_rob001_normal.typescript`, `07_rob001_asan.typescript`, `09_rob001_ubsan.typescript`; confirma lectura fuera del subobjeto, no contrato de rechazo |
| ROB-001, UT-REG-008 | Hipotesis ROB-001; banco interno de 32 bytes | CORE mock; `start=31,count=2`; values=`{0x12,0x34}` | Conservar status y efectos exactos sin asumir rechazo | Retorna OK; escribe `0x12` en indice 31 y `0x34` en el byte inmediatamente posterior; los demas bytes/canarios no cambian | `PASS`; 0 normal/ASan/UBSan | `SUSPECTED DEFECT` | Mismos transcripts; ASan/UBSan no detectan el acceso intraobjeto |
| ROB-002, casos core existentes | REQ-004/007/008 y ROB-002 | Fallos CMock en RREG, WREG, presence, channel y RDATA1 | Propagar SPI_TRANSFER, preservar/zero output segun funcion y suprimir operaciones posteriores | Todos los casos core reproducidos pasan; channel suprime read/START1 y sample/presence dejan estructuras zeroed donde la fuente limpia antes | `PASS`; 0 | `EXPECTED BEHAVIOR` para casos core | Runs baseline/final en `../../04_unitarias/milestone_3a_20261015T150616Z/`; adapters y CFT siguen `NOT RUN` |
| ROB-003, UT-DRDY-002..007 | REQ-006 y ROB-003 | Ready, delayed, GPIO_READ/timeout seguidos por SPI success/failure | Registrar por separado retorno, SPI, waits y muestra | Ready/delayed OK; GPIO_READ+SPI OK retorna OK con `drdy=-1/status=GPIO_READ`; timeout+SPI OK retorna OK con `drdy=1/status=DRDY_TIMEOUT`; si SPI falla retorna SPI_TRANSFER y muestra zeroed | `PASS`; 0 | `CONTRACT AMBIGUITY` | Runs final; no contrato aprobado exige propagacion fatal de DRDY |
| ROB-003, UT-ACQ-ROB003-001..004 | ROB-003, consecuencia acquisition | N=1, rate=2; muestra diagnostica o SPI_TRANSFER | Separar salida/retorno/cleanup para las cuatro combinaciones | Muestras GPIO_READ/DRDY_TIMEOUT se emiten y acquisition retorna 0; SPI_TRANSFER suprime output, retorna -1 y cierra ADC | `PASS`; 0 | `CONTRACT AMBIGUITY` | `../../04_unitarias/.../final/08_acquisition_test_run_1.typescript` y `10_acquisition_test_run_2.typescript`; acquisition no puede distinguir la causa DRDY cuando la API solo retorna SPI_TRANSFER |
| DSP-017, UT-ACQ-DSP017-001 | REQ-008; rango CLI rate `1..10` | N=2, rate=1; wrapper falla dos veces con EINVAL y luego permite terminar | Capturar argumento y retries sin loop no acotado | Tres llamadas reciben siempre `{tv_sec=0,tv_nsec=1000000000}` | `PASS`; 0 | `CONFIRMED DEFECT` DEF-001 | Runs acquisition; el tercer retorno exitoso es una barrera de test, no comportamiento POSIX real |
| DSP-017, UT-ACQ-DSP017-002 | POSIX `timespec`; REQ-008 | Llamada host directa con `{0,1000000000}` | POSIX rechaza `tv_nsec >= 1000000000` | `nanosleep` retorna -1 y `errno=EINVAL` | `PASS`; 0 | `CONFIRMED DEFECT` DEF-001 | Runs acquisition; resultado host, no medicion temporal target |
| DSP-017, PROC-DSP017-001 | REQ-008 y rate CLI valido | `acquisition_run_samples`, N=2, rate=1, fakes deterministas, wait real | Terminar con exactamente dos muestras para entrada valida | No retorna antes del timeout externo de 2 s; proceso terminado | `FAIL`; 124 | `CONFIRMED DEFECT` DEF-001 | `02_dsp017_probe_build_pass.typescript`; `03_dsp017_probe_timeout.typescript`; proceso siempre acotado |

## Hallazgo de sanitizer

`FND-ROB001-001` afecta `ads1263.c:102-108,136-142`. Es reproducible en normal, ASan y UBSan: el rango combinado no se valida y el mock lee/escribe el byte posterior a `mock_registers[31]`. Impacto: divulgacion o corrupcion de estado adyacente dentro de `ads1263_t` para una solicitud crossing-range. Disposicion: `SUSPECTED DEFECT`, porque el efecto inseguro esta observado pero no existe contrato aprobado para el status requerido ante ese input. ASan y UBSan terminan 0 sin diagnostico; al permanecer dentro del objeto contenedor, ese silencio no prueba seguridad.

## Sanitizers

| Perfil | Comando | Casos | Resultado | Limitacion |
|---|---|---:|---|---|
| Normal aislado | `timeout ... ceedling test:test_ads1263_robustness` | 2 | 2/2 PASS, exit 0 | Assertions explicitas detectan efecto logico |
| ASan | `timeout ... ceedling test:test_ads1263_robustness --mixin=test/ceedling/mixins/asan.yml` | 2 | 2/2 PASS, exit 0, sin diagnostico | No detecta necesariamente cruces entre subobjetos del mismo struct |
| UBSan | `timeout ... ceedling test:test_ads1263_robustness --mixin=test/ceedling/mixins/ubsan.yml` | 2 | 2/2 PASS, exit 0, sin diagnostico | No modela el limite logico del arreglo embebido |

## Incidencias de infraestructura

- `01_dsp017_probe_build.typescript` conserva el primer enlace fallido por faltar el fake de `ads1263_read_drdy`; `02_dsp017_probe_build_pass.typescript` conserva la correccion test-only.
- El primer desarrollo del proyecto acquisition uso nombres CMock no generados y luego objetos ReturnThruPtr de vida automatica; esos intentos ocurrieron antes del run formal y no tuvieron transcript raw. Se corrigieron sin tocar produccion; todos los runs formales posteriores estan preservados.
- El primer export completo gcovr ADS1263 se lanzo concurrentemente con el resumen y fallo por carrera sobre archivos gcov. El fallo se conserva en `../../06_cobertura/.../final/04_ads1263_gcovr_artifacts.typescript` y el rerun serial exitoso en `05_ads1263_gcovr_artifacts_pass.typescript`.

## Gate

No se modifico produccion. DEF-001 requiere decision humana sobre correccion en un cambio separado. ROB-001 requiere aprobar el contrato crossing-range antes de clasificar el comportamiento como defecto confirmado o definir su correccion. ROB-003 conserva su ambiguedad contractual.
