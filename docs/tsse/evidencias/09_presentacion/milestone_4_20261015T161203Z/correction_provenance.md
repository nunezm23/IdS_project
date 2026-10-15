# Generation provenance addendum

## Corrected generation

- UTC completion timestamp: `2026-10-15T17:06:13Z`.
- Git HEAD: `54a91ed4b684b5ac6f4221c8743768fad5083270`.
- Worktree: `DIRTY`. Modified and untracked files existed before and during generation; this was not a clean-checkout build. The status captured during the correction workflow is preserved in `correction_git_status.stdout.txt`.
- Host: `cronos`.
- Kernel: `Linux 7.0.0-38-generic #38-Ubuntu SMP PREEMPT_DYNAMIC Fri Sep 4 09:10:14 UTC 2026 x86_64 GNU/Linux`.
- Python: `CPython 3.14.4`.
- Python ZIP implementation: standard-library `zipfile` from `/usr/lib/python3.14/zipfile/__init__.py`.
- Package verifier: `UnZip 6.00 of 20 April 2009, by Debian`.
- Git: `git version 2.53.0`.
- Generator dependencies: Python standard library only; no package installation was performed.

Exact generation command, run from repository root:

```bash
python3 docs/tsse/presentacion/generate_presentation.py --output docs/tsse/presentacion/verificacion_software_ads1263_milestone_4.pptx
```

- Output: `docs/tsse/presentacion/verificacion_software_ads1263_milestone_4.pptx`.
- SHA-256: `667cb94f4905ea45bb6036325b2192c9f76b10d429674401772f2a110d4228cc`.
- Snapshot post-generacion: `presentation_snapshot_checksums.typescript` identifica por SHA-256 el generador dirty, guion, banco de preguntas y PPTX resultante. Se capturo a `2026-10-15T17:13:35Z` sin cambios posteriores en esos inputs y no se presenta como snapshot pre-generacion.
- Generation exit code: `0`.
- Deterministic regeneration exit code: `0`; the second generation produced the same SHA-256.

## Human and visual status

- Google Slides import: `NOT PERFORMED`.
- Visual review: `NOT PERFORMED`.
- Human rehearsal or delivery: `BLOCKED / PENDING HUMAN DELIVERY`.

This addendum records automated package and content checks only. It does not claim visual fidelity in a presentation application or human delivery quality.
