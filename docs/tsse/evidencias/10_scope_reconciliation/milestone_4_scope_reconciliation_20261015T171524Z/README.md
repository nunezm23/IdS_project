# OpenSpec scope reconciliation provenance

- Change: `tsse-embedded-software-testing`.
- Pre-reconciliation Git HEAD: `9678a8e82d3a2aa01b3b279f9011627529ac553e`.
- Pre-reconciliation worktree: clean; `git status --short` produced no output immediately before the historical snapshot was created.
- Original OpenSpec status: `63/67` tasks complete, reported by `openspec list --json` before any planning-artifact edit.
- Historical checklist: `tasks_63_of_67_historical.md`.
- Historical checklist SHA-256: `c8a27c81317c19cb699773a8541e3eb8313a4ecfd5f2fe5b6f797ecd3e93a645`.
- Historical checklist size: `16538` bytes.
- Byte-identity check: `cmp -s` against the active pre-reconciliation `tasks.md` returned exit code `0`; both files reported `16538` bytes.

The active task-count change that follows is a scope reassignment, not loss of implementation work. Sections 14 and 15 are reassigned to the human user and ChatGPT web workflow for a separate LaTeX academic report and Google Slides presentation. Their exact original text remains in the historical checklist above.

- Post-reconciliation OpenSpec status: `59/61`, independently reported by `openspec list --json` after removing exactly six academic-authoring tasks (four complete and two incomplete) from the active checklist.

Previously generated report, PPTX, speaker-note, question-bank, generator, and presentation-evidence files are historical material. This reconciliation does not delete, overwrite, regenerate, validate visually, approve, or claim delivery of those artifacts.
