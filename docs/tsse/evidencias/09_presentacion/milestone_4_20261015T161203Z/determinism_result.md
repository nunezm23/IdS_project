# Determinism check

- Initial generation SHA-256: `03eb4e3402d63fd3afc604b63c869ca4afd0994a0984096a39edcaf74379a8d9`.
- Regeneration SHA-256: `03eb4e3402d63fd3afc604b63c869ca4afd0994a0984096a39edcaf74379a8d9`.
- Result: `PASS`, byte-identical output for the retained generator and inputs.

See `generation.stdout.txt` and `regeneration.stdout.txt`.

## Addendum - corrected deck

- Corrected generation UTC timestamp: `2026-10-15T17:06:13Z`.
- Initial corrected generation SHA-256: `667cb94f4905ea45bb6036325b2192c9f76b10d429674401772f2a110d4228cc`.
- Corrected regeneration SHA-256: `667cb94f4905ea45bb6036325b2192c9f76b10d429674401772f2a110d4228cc`.
- Exact comparison command: `cmp -s docs/tsse/evidencias/09_presentacion/milestone_4_20261015T161203Z/correction_initial.sha256 docs/tsse/evidencias/09_presentacion/milestone_4_20261015T161203Z/correction_regeneration.sha256`.
- Result: `PASS` (exit code `0`), byte-identical output for the corrected generator and inputs.

See `correction_generation.stdout.txt`, `correction_regeneration.stdout.txt`, `correction_initial.sha256`, and `correction_regeneration.sha256`.
