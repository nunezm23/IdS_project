# Matriz de trazabilidad — TSSE

## Referencia
- Proyecto: IdS_project
- Commit base: c59b240
- Rama: feature/tsse-testing
- Estado: planificación; pruebas aún no ejecutadas

## Relación entre requisitos y pruebas

| Requisito | Descripción resumida | Prueba prevista | Nivel | Estado |
|---|---|---|---|---|
| REQ-001 | Ejecución en STM32MP157D | No aplica al alcance actual | HW/SW | Excluido |
| REQ-002 | Comunicación SPI con ADS1263 | UT-SPI: comandos y transferencias simuladas | Unitario | Pendiente |
| REQ-003 | Configuración SPI predeterminada | UT-CFG: configuración y argumentos | Unitario | Pendiente |
| REQ-004 | Lectura de registros 0x00..0x04 | UT-REG: lectura y límites | Unitario | Pendiente |
| REQ-005 | Detección de respuesta 0x00/0xFF | UT-PRES: detección de dispositivo | Unitario | Pendiente |
| REQ-006 | Lectura DRDY por GPIO | UT-DRDY: respuestas simuladas | Unitario | Pendiente |
| REQ-007 | Adquisición del canal 0 | CFT-ACQ: flujo de adquisición | Diseño de sistema | Pendiente |
| REQ-008 | Adquisición de N muestras | INT-ACQ: conteo y secuencia | Integración host | Pendiente |
| REQ-009 | Adquisición continua | Diseño documentado; ejecución opcional | Sistema | Parcial planificado |
| REQ-010 | Exportación CSV | INT-CSV: salida simulada | Integración host | Pendiente |
| REQ-011 | Rechazo de parámetros inválidos | CFT-ACQ y UT de límites | Unitario / sistema | Pendiente |
| REQ-012 | Aislamiento de código Linux | Revisión de arquitectura | Estático | Pendiente |

Nota: la ejecución de tests con mocks verifica contratos
de software, no la comunicación física con el ADS1263.

## Casos adicionales de robustez

| ID | Objetivo | Estado |
|---|---|---|
| ROB-001 | Rechazar acceso a registros fuera de rango | Pendiente |
| ROB-002 | Verificar tratamiento de fallas SPI | Pendiente |
| ROB-003 | Caracterizar propagación de errores DRDY | Pendiente |
| ROB-004 | Verificar conversión ADC para valores límite | Pendiente |

## Criterios de trazabilidad

- Cada prueba implementada tendrá un identificador.
- Los resultados se registrarán como PASS, FAIL o BLOCKED.
- No se afirmará cumplimiento de requisitos de hardware.
- Los casos adicionales de robustez se distinguirán
  de los requisitos originales.
- Se actualizará esta matriz después de ejecutar las pruebas.
