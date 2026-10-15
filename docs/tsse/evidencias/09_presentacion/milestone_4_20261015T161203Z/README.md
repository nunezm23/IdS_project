# Evidencia de presentación - Milestone 4

Run ID: `milestone_4_20261015T161203Z`

## Alcance

Este directorio preserva evidencia de generación y validación técnica del deck. No demuestra entrega oral, ensayo humano ni revisión visual en Google Slides.

## Entregables relacionados

| Artefacto | Ruta |
|---|---|
| Deck OOXML | `docs/tsse/presentacion/verificacion_software_ads1263_milestone_4.pptx` |
| Generador determinista | `docs/tsse/presentacion/generate_presentation.py` |
| Guion de 10 minutos | `docs/tsse/presentacion/guion_orador_10_min.md` |
| Banco de preguntas | `docs/tsse/presentacion/banco_preguntas.md` |
| Estado/plantilla de ensayo | `rehearsal_status.md` |

## Evidencia preservada

| Archivo | Propósito |
|---|---|
| `generation.command.txt` | Comando exacto de generación |
| `generation.stdout.txt` | Salida estándar del generador |
| `generation.stderr.txt` | Salida de error del generador |
| `generation.exitcode.txt` | Código de salida |
| `verification.command.txt` | Comando exacto de verificación independiente |
| `verification.stdout.txt` | CRC, XML, inventario y SHA-256 |
| `verification.stderr.txt` | Salida de error de verificación |
| `verification.exitcode.txt` | Código de salida |
| `deck.sha256` | Checksum SHA-256 del `.pptx` |
| `checksum_verify.*` | Verificación del checksum publicado |
| `regeneration.*` | Segunda generación sobre el mismo destino |
| `determinism_result.md` | Comparación de hashes inicial/final |
| `zip_test.*` | Verificación adicional con `unzip -t` |

Los archivos originales anteriores a la corrección se conservan sin cambios. Los archivos `correction_*` son el addendum de regeneración y verificación del `2026-10-15T17:06:13Z`; `deck.sha256` conserva los hashes reemplazados como comentarios y deja un único hash actual activo.

| Addendum de corrección | Propósito |
|---|---|
| `correction_provenance.md` | Timestamp UTC, HEAD, estado dirty, host/kernel, herramientas, comando y salida |
| `correction_generation.*` | Generación corregida, salida y código de retorno |
| `correction_verification.*` | Verificación estructural, XML, relaciones, inventario, contenido y formato |
| `correction_zip_test.*` | Prueba independiente del paquete ZIP |
| `correction_checksum_verify.*` | Verificación del checksum actual publicado |
| `correction_initial.sha256` | Hash de la primera generación corregida |
| `correction_regeneration.*` | Segunda generación corregida y su hash |
| `correction_determinism.*` | Comparación exacta de ambos registros SHA-256 |
| `correction_git_status.stdout.txt` | Estado dirty capturado durante el workflow |
| `presentation_snapshot_checksums.typescript` | Snapshot SHA-256 post-generación del generador dirty, guion, banco y PPTX |

Los comandos, logs y hashes previos a la corrección se preservan, pero el binario PPTX previo fue reemplazado en el mismo destino y no está disponible como artefacto separado; sólo permanecen sus hashes supersedidos. `deck.sha256` también fue actualizado para dejar un único hash activo y conservar los anteriores como comentarios. Esta excepción append-only no se oculta ni se declara subsanada retroactivamente.

## Criterios técnicos

- Generación únicamente con Python y módulos de biblioteca estándar: `zipfile`, `xml.etree.ElementTree`, `hashlib`, `argparse` y utilidades estándar.
- Sin instalación de paquetes.
- Contenedor ZIP con CRC válido.
- Todas las partes `.xml` y `.rels` parseables.
- Partes OOXML principales presentes.
- 16 slides: 9 principales y 7 de respaldo.
- 16 notes slides con claims y rutas de evidencia.
- Formato panorámico 16:9.
- Todos los targets de relaciones internas existen en el paquete.
- Regeneración byte-idéntica con el mismo SHA-256.

## Límites de verificación

- La estructura técnica y el XML se verifican automáticamente.
- La importación y revisión visual en Google Slides requieren una acción humana fuera de esta ejecución.
- El estado de ensayo permanece `BLOCKED / PENDING HUMAN DELIVERY` hasta que una persona complete `rehearsal_status.md`.
- No se editó OpenSpec ni se realizó commit como parte de estos entregables.

## Estado humano

- No se importó el deck en Google Slides y no se realizó revisión visual.
- No se realizó ensayo ni entrega oral humana.
- El estado se mantiene `BLOCKED / PENDING HUMAN DELIVERY`.
