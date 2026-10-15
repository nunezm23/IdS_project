# Revision independiente del modelo CFT - Milestone 1

## Alcance y estado

- Actividad: inspeccion fuente-modelo; no es ejecucion de pruebas.
- Fuente preservada: `components/acquisition/src/acquisition.c` del baseline `c59b24017f5daebb4e37512d799a0d07f589bce5`.
- Modelo revisado: `docs/tsse/cft_acquisition_run_samples.md`.
- Grafo revisado: `docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.dot`.
- Frontera: `acquisition_run_samples()` y todos sus helpers locales alcanzables; dependencias ADS1263, logger, archivo y tiempo como contratos.

## Metodo

Un revisor independiente comparo la fuente con la tabla de decisiones y el DOT. La inspeccion cubrio helpers incluidos, short-circuit, edges, outcomes externos, cleanup, observables, referencias de linea y calculo de complejidad.

## Hallazgos y correcciones

| ID | Hallazgo inicial | Correccion | Estado |
|---|---|---|---|
| CFT-R01 | La tabla de contratos omitia `ads1263_status_to_string()` y `ads1263_close()` | Se agregaron outcomes, limites y call sites; los nodos de close se identifican como contratos | CORREGIDO |
| CFT-R02 | N99 no tenia referencia y contradecia la afirmacion de mapeo completo | N99 se declara join sintetico de los returns en lineas 267, 276 y 282 | CORREGIDO |
| CFT-R03 | D16-D18 mezclaban rangos de seleccion y acciones del `switch` | Se separaron los selectores 42-43, 42/47 y 42/51/61 de P08-P12 | CORREGIDO |

## Recalculo independiente

- Decisiones binarias atomicas: `3 + 7 + 10 + 12 = 32`.
- Nodos DOT: `N=75`.
- Edges DOT: `E=106`.
- Componentes conectados: `P=1`.
- Complejidad: `V(G)=E-N+2P=106-75+2=33`, equivalente a `D+1`.

## Disposicion

`APROBADO` despues de corregir CFT-R01..CFT-R03. La revision confirma correspondencia con la fuente para la frontera declarada. No extiende la afirmacion a internals ADS1263/HAL, hardware, timing real, persistencia fisica de CSV ni paths ambientales no acotados.
