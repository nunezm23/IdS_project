# Milestone 4 static-analysis evidence

Run ID: `milestone_4_20261015T161203Z`

Repository-relative evidence root: `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/`

## Outcome

| Activity/configuration | Execution state | Result | Evidence |
|---|---|---|---|
| Metadata, source inventory, checksums, policy | Executed first | Complete | `00_policy.md`, `01_capture_metadata.sh`, `raw_metadata/` |
| Cppcheck, `ADS1263_NO_GPIOD=1`, required profile | Executed | Completed with findings, not clean | `raw_analysis/001_cppcheck_no_gpiod.*` |
| Cppcheck, `ADS1263_NO_GPIOD=1`, exhaustive follow-up | Executed | Completed with findings, not clean | `03_exhaustive_followup_policy.md`, `raw_analysis_exhaustive/` |
| Cppcheck, `ADS1263_NO_GPIOD=0`, libgpiod v2 | Not run | `BLOCKED` only for this configuration | `raw_metadata/015_*`, `017_*`, `019_*`; `raw_analysis/002_*` |
| GCC 15 informational, `ADS1263_NO_GPIOD=1` | Executed | Exit 0, zero GCC diagnostics; not an acceptance result | `raw_analysis/003_gcc15_no_gpiod.*` |
| GCC 15 informational, `ADS1263_NO_GPIOD=0`, libgpiod v2 | Not run | `BLOCKED` only for this configuration | `raw_analysis/004_*` |

The supported no-gpiod Cppcheck configuration is not described as clean because it emitted diagnostics. The unsupported libgpiod-v2 configuration is not described as clean because it was not run.

## Provenance

| Field | Captured value | Raw evidence |
|---|---|---|
| Capture began | `2026-10-15T16:14:36Z` | `raw_metadata/001_capture_clock.stdout.txt` |
| Branch | `feature/tsse-testing` | `raw_metadata/002_git_branch.stdout.txt` |
| HEAD | `54a91ed4b684b5ac6f4221c8743768fad5083270` | `raw_metadata/003_git_head.stdout.txt` |
| Host | Ubuntu 26.04.1 LTS, Linux 7.0.0-38-generic, x86_64, 64-bit | `raw_metadata/007_*`, `008_*`, `009_*` |
| Cppcheck | 2.19.0 | `raw_metadata/010_*`, `011_*` |
| GCC 15 | 15.2.0 (`gcc-15`) | `raw_metadata/012_*`, `013_*` |
| Production scope | 6 C and 6 H files | `raw_metadata/020_source_inventory.stdout.txt` |
| Initial source hashes | SHA-256 for all 12 files | `raw_metadata/021_source_sha256.stdout.txt` |
| Post-run source hashes | Identical SHA-256 values for all 12 files | `raw_analysis/005_source_sha256_after.stdout.txt` |
| Production/Makefile diff | Empty before and after execution | `raw_metadata/005_*`, `006_*`; `raw_analysis/006_*` |
| Dependency classification | Real host tools/headers; no mocks, simulation, hardware, or package installation | This file and `00_policy.md` |

The initial worktree snapshot was taken after creation of the policy, collector, and early metadata files; at that instant it listed only the partial untracked run set. The later snapshot in `raw_final/007_final_worktree.stdout.txt` also lists tracked and untracked test/integration changes outside this directory. These snapshots establish observed states, not who created concurrent changes or exactly when they appeared. The evidence process did not intentionally edit those paths. The narrower production C/H and Makefile worktree/index diff artifacts are empty before and after analysis.

## Configuration gate

The libgpiod-v2 branch required both package metadata and header availability. It failed both prerequisites:

| Gate | Exit | Observation |
|---|---:|---|
| `pkg-config --version` | 127 | `pkg-config` command unavailable |
| `pkg-config --exists "libgpiod >= 2.0"` | 127 | Package/version gate unavailable |
| `#include <gpiod.h>` GCC syntax probe | 1 | `gpiod.h: No such file or directory` |

No package was installed. Consequently, only `ADS1263_NO_GPIOD=0` is `BLOCKED`; `ADS1263_NO_GPIOD=1` was analyzed normally.

## Finding counts

### Cppcheck

The primary result is the exhaustive follow-up:

| Category | Count | Interpretation |
|---|---:|---|
| Style source diagnostics | 41 | Individually listed as `SA-CPPCHECK-001` through `SA-CPPCHECK-041` |
| `missingIncludeSystem` information | 32 | Individually indexed as `SA-ENV-001` through `SA-ENV-032`; retained analysis limitation |
| `checkersReport` information | 1 | Tool metadata, not a source finding |
| Error, warning, performance, portability diagnostics | 0 | No diagnostic in these severities; this is not a defect-free claim |
| Confirmed production defects | 0 | All source diagnostics were dispositioned as context false positives or accepted interface-design observations |

The initial mandatory run additionally emitted two `normalCheckLevelMaxBranches` notices, `SA-ENV-033` and `SA-ENV-034`. The exhaustive follow-up resolved that limitation without overwriting the initial run.

### GCC 15

The informational no-gpiod command returned 0 with empty stdout and stderr: zero GCC warnings and zero GCC analyzer diagnostics. GCC results remain separate from Cppcheck and are not acceptance criteria. The libgpiod-v2 GCC profile is `BLOCKED` by the shared gate.

### Consolidated dispositions

| Record type | Count |
|---|---:|
| Unique Cppcheck source findings | 41 |
| Cppcheck/environment limitation records | 34 |
| Configuration blocker records | 1 |
| GCC source findings | 0 |
| Confirmed defects | 0 |
| Total stable records in `findings.md` | 76 |

Repeated diagnostics in the mandatory and exhaustive runs are one finding, not counted twice.

## OpenSpec task eligibility

| Task | Evidence assessment | Tracking state |
|---|---|---|
| 12.1 | Evidence complete and eligible: version, exact scope, includes, platform, language, checks, no-suppression policy, defines, and exit policy were documented before interpretation | Checkbox intentionally unchanged |
| 12.2 | Evidence complete and eligible: no-gpiod ran over all 12 files; libgpiod-v2 gate failure and `BLOCKED` status are preserved | Checkbox intentionally unchanged |
| 12.3 | Evidence complete and eligible: every source diagnostic and environment limitation has a stable record and disposition; no incomplete/blocked run is called clean | Checkbox intentionally unchanged |
| 12.4 | Evidence complete and eligible: optional GCC 15 profile ran for the available configuration without `-Werror` and remains informational/separate | Checkbox intentionally unchanged |

OpenSpec remains `44/67` with tasks 12.1-12.4 textually unchecked because the user prohibited edits outside this directory. This run supplies evidence for a later authorized checkbox-only update; it does not itself modify OpenSpec.

## Commands

Every metadata probe and analyzer command executed through a collector's `record()` function has adjacent `.command.txt`, `.stdout.txt`, `.stderr.txt`, `.exitcode.txt`, `.started_utc.txt`, and `.finished_utc.txt` artifacts. Blocked commands were not executed and intentionally have only `command_not_run`, reason, and status records. The original finalizer's built-in `008` through `010` aggregation records did not all have timestamps; `07_integrity_addendum.md` documents that limitation rather than retrofitting historical raw evidence. Key command records:

| Purpose | Exact-command artifact |
|---|---|
| Metadata and tool probes | `raw_metadata/*.command.txt` |
| Required Cppcheck run | `raw_analysis/001_cppcheck_no_gpiod.command.txt` |
| Blocked Cppcheck command | `raw_analysis/002_cppcheck_libgpiod_v2.command_not_run.txt` |
| Informational GCC 15 run | `raw_analysis/003_gcc15_no_gpiod.command.txt` |
| Blocked GCC 15 command | `raw_analysis/004_gcc15_libgpiod_v2.command_not_run.txt` |
| Exhaustive Cppcheck follow-up | `raw_analysis_exhaustive/001_cppcheck_exhaustive_no_gpiod.command.txt` |

Collector invocations from repository root:

```sh
bash docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/01_capture_metadata.sh
bash docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/02_execute_analysis.sh
bash docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/04_execute_exhaustive_followup.sh
bash docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/05_finalize_evidence.sh
bash docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/06_integrity_review.sh pre-correction
bash docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/06_integrity_review.sh finalize
```

The collectors are intentionally not rerunnable in place: each exits 99 if its output directory already exists. A repeat execution requires a new run ID, preserving append-only evidence.

## Artifact map

| Path | Purpose |
|---|---|
| `00_policy.md` | Policy fixed before analyzer interpretation |
| `01_capture_metadata.sh` | Append-only metadata/provenance collector |
| `02_execute_analysis.sh` | Append-only required Cppcheck, gate, GCC, and post-run-integrity collector |
| `03_exhaustive_followup_policy.md` | Pre-execution rationale and exact exhaustive command |
| `04_execute_exhaustive_followup.sh` | Append-only exhaustive Cppcheck collector |
| `05_finalize_evidence.sh` | Append-only final counts, source-integrity, worktree, inventory, and evidence-checksum collector |
| `06_integrity_review.sh` | Append-only historical-manifest verification and corrected final inventory/checksum collector |
| `07_integrity_addendum.md` | Review corrections, old-manifest scope, and new self-exclusion rules |
| `findings.md` | Stable finding, limitation, blocker, and risk dispositions |
| `raw_metadata/` | Exact metadata commands, output streams, times, and exits |
| `raw_analysis/` | Exact mandatory Cppcheck/GCC evidence and blocked-command records |
| `raw_analysis_exhaustive/` | Exact exhaustive follow-up evidence |
| `raw_final/` | Final audit, inventory, integrity, and checksums |
| `integrity_review_20261015T162917Z/` | Pre-correction verification plus the final corrected complete inventory/checksum set |

## Limitations

- Cppcheck does not load the listed system headers under this invocation; its own message says standard headers are not required for proper results, but this remains visible as an analysis limitation and likely contributes to the hidden-kernel-write const false positive.
- Directly passing headers is required by the declared 6 C/6 H scope, but causes standalone-header whole-program checks to report members used by production translation units as unused.
- The libgpiod implementation was not analyzed because both required availability gates failed. No conclusion is made about that branch.
- Static analysis and host syntax analysis do not verify physical SPI/GPIO behavior, target execution, ADC correctness, or absence of all defects.
