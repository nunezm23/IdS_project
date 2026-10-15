# Presentación TSSE - Milestone 4

## Entregables

- `verificacion_software_ads1263_milestone_4.pptx`: 9 láminas principales y 7 de respaldo, formato 16:9, con notas del orador embebidas.
- `guion_orador_10_min.md`: guion detallado de 10:00 minutos y enlaces de evidencia por lámina.
- `banco_preguntas.md`: preguntas probables, respuestas breves y evidencia de soporte.
- `generate_presentation.py`: generador OOXML determinista que usa únicamente la biblioteca estándar de Python.

La evidencia de generación, validación, checksum y estado de ensayo está en:

`docs/tsse/evidencias/09_presentacion/milestone_4_20261015T161203Z/`

## Regeneración

Desde la raíz del repositorio:

```bash
python3 docs/tsse/presentacion/generate_presentation.py \
  --output docs/tsse/presentacion/verificacion_software_ads1263_milestone_4.pptx
```

El generador fija metadata y timestamps ZIP, ordena las partes OOXML y produce bytes reproducibles para el mismo código.

## Verificación

```bash
python3 docs/tsse/presentacion/generate_presentation.py \
  --verify-only \
  --output docs/tsse/presentacion/verificacion_software_ads1263_milestone_4.pptx
```

La verificación comprueba CRC ZIP, partes obligatorias, parseo de todos los XML, 16 slides y 16 notes slides. La importación visual final en Google Slides requiere revisión humana.
