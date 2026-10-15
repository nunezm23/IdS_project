# Entorno y procedencia - Milestone 4

- Run set: `milestone_4_20261015T161203Z`.
- Rama: `feature/tsse-testing`; HEAD: `54a91ed4b684b5ac6f4221c8743768fad5083270`.
- Identidad completa de las entradas formales: HEAD anterior mas los inputs test/infraestructura dirty enumerados con SHA-256 en `test_input_checksums.typescript`. HEAD por si solo no reproduce las ejecuciones.
- Baseline de produccion: `c59b24017f5daebb4e37512d799a0d07f589bce5`.
- `production_guard_before.typescript`: diff de C/H/Makefile contra baseline vacio, exit 0. El analisis estatico vuelve a verificar hashes y diff al cierre.
- Entorno: Linux x86-64, GCC 15.2.0, Ceedling 1.1.10. Los transcripts del directorio conservan versiones, rama, HEAD y worktree.

`test_input_checksums.typescript` fue capturado despues de las ejecuciones formales sobre los mismos inputs de prueba, que permanecieron sin cambios hasta esa captura. No existe un commit ni un manifest de checksums pre-ejecucion para esos cambios test/infraestructura; el transcript identifica exactamente el estado usado, pero su momento de captura debe conservarse como limitacion de procedencia.

La evidencia es host/software-only. No se uso hardware ni se verificaron target, bus fisico, GPIO, exactitud ADC o tiempo real.
