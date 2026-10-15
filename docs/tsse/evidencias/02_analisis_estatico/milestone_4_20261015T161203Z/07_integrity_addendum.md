# Integrity and provenance review addendum

Review ID: `integrity_review_20261015T162917Z`

This addendum corrects summary and integrity overclaims discovered after the static-analysis evidence was collected. It does not replace, delete, or rewrite any raw analyzer output or historical `raw_metadata/`, `raw_analysis/`, `raw_analysis_exhaustive/`, or `raw_final/` result.

## Worktree-state correction

`raw_metadata/004_git_status.stdout.txt` is an initial observation made after the policy, metadata collector, and some metadata outputs already existed. It lists only that partial evidence set as untracked at that instant. It does not prove that test paths were unchanged before evidence collection began.

`raw_final/007_final_worktree.stdout.txt` is a later observation. It records tracked changes in three test-support/configuration paths and additional untracked test/integration paths outside this evidence directory. The two snapshots establish different observed states but do not establish authorship or an exact change time. This evidence work did not intentionally edit those paths. The narrower production C/H and Makefile diffs in `raw_metadata/005_*`, `raw_metadata/006_*`, `raw_analysis/006_*`, and `raw_final/005_*` remain empty.

## Per-command metadata correction

The six-file command metadata statement applies to commands executed through collector `record()` functions: command, stdout, stderr, exit code, start UTC, and finish UTC. It does not apply to commands blocked before invocation; those have `command_not_run`, reason, and status artifacts. The original finalizer's built-in diagnostic count, finding count, and inventory steps also lacked uniform start/finish records. The corrected `05_finalize_evidence.sh` adds those records for a future fresh run ID. Historical files are left unchanged.

## Historical inventory and manifest scope

The historical inventory `raw_final/010_evidence_inventory.stdout.txt` contains 232 paths but was snapshotted before its own four `010_evidence_inventory.*` files and all six `011_evidence_sha256.*` files existed. It therefore omitted ten files and is not a complete final inventory.

The historical checksum manifest `raw_final/011_evidence_sha256.stdout.txt` contains 238 entries. It covers the four `010_evidence_inventory.*` files plus `011_evidence_sha256.command.txt` and `011_evidence_sha256.started_utc.txt`, but omits these four files:

```text
raw_final/011_evidence_sha256.stdout.txt
raw_final/011_evidence_sha256.stderr.txt
raw_final/011_evidence_sha256.exitcode.txt
raw_final/011_evidence_sha256.finished_utc.txt
```

The historical command description mentioned only self-exclusion of the checksum output and therefore understated the actual exclusions. The old manifest verified successfully before the review corrections, as preserved by `integrity_review_20261015T162917Z/001_old_manifest_pre_correction_verify.*` with exit 0. After the intentional corrections to `00_policy.md`, `README.md`, and `05_finalize_evidence.sh`, those three old entries are expected to differ; the post-correction verification is retained without rewriting the old manifest.

## Corrected final integrity set

`integrity_review_20261015T162917Z/final/010_complete_inventory.txt` is generated only after every final artifact path is precreated. It includes every regular file under this run directory at the inventory instant, including itself, the historical bundle, this addendum, corrected scripts/documents, and all final integrity output paths.

`integrity_review_20261015T162917Z/final/011_complete_sha256.txt` hashes every inventory entry except exactly the eight paths listed in `final/009_checksum_exclusions.txt`:

```text
final/011_complete_sha256.txt
final/011_complete_sha256.stderr.txt
final/011_complete_sha256.exitcode.txt
final/011_complete_sha256.finished_utc.txt
final/012_complete_sha256_verify.stdout.txt
final/012_complete_sha256_verify.stderr.txt
final/012_complete_sha256_verify.exitcode.txt
final/012_complete_sha256_verify.finished_utc.txt
```

The manifest itself is excluded to avoid self-reference. Its stderr, exit, and finish files are excluded because they are written after hashing. The verification stdout, stderr, exit, and finish files are excluded because they are written after the manifest. The checksum command/start records, verification command/start records, exclusion list, coverage counts, complete inventory, script, corrected documents, addendum, and all historical raw evidence are included. `final/013_coverage_counts.txt` states the exact inventory, manifest, and exclusion counts.

## Hash-claim verification

The final review separately verifies:

- The old evidence manifest before corrections, preserving its original validity and actual scope.
- The old evidence manifest after corrections, preserving expected mismatches rather than pretending it covers revised files.
- Each of the three recorded production-source SHA-256 lists against the current 12 source files.
- The new complete checksum manifest against every path it claims to cover.

No checksum manifest claims to hash itself or a verification result generated after that manifest.
