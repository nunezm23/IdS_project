# TSSE technical evidence handoff index

## Purpose and ownership

This document is a factual index to retained software-testing evidence. It is not the final academic report, a slide deck, speaker notes, or evidence of human approval or rehearsal.

The final academic report in LaTeX and the final Google Slides presentation are owned by the human user and ChatGPT web workflow. Previously generated report and presentation files remain historical material and were not modified or regenerated during this reconciliation.

The official academic rubric and its exact four-objective wording are not stored in this repository (`openspec/changes/tsse-embedded-software-testing/design.md` and `docs/tsse/evidencias/01_proyecto/milestone_1_revision_plan.md`). The four groupings below therefore derive from the approved project proposal, not from an invented official quotation.

## Baselines and provenance

| Item | Value | Evidence |
|---|---|---|
| Protected production baseline | `c59b24017f5daebb4e37512d799a0d07f589bce5` | `docs/tsse/evidencias/01_proyecto/milestone_1_linea_base.md` |
| Formal Milestone 4 execution HEAD | `54a91ed4b684b5ac6f4221c8743768fad5083270` plus dirty test/infrastructure inputs | `docs/tsse/evidencias/01_proyecto/milestone_4_20261015T161203Z/README.md` |
| Exact post-run test/input hashes | Captured after execution from reportedly unchanged inputs; not a pre-run commit | `docs/tsse/evidencias/01_proyecto/milestone_4_20261015T161203Z/test_input_checksums.typescript` |
| Scope-reconciliation base HEAD | `9678a8e82d3a2aa01b3b279f9011627529ac553e`, clean before reconciliation edits | `docs/tsse/evidencias/10_scope_reconciliation/milestone_4_scope_reconciliation_20261015T171524Z/README.md` |
| Original OpenSpec checklist | `63/67`, 16538 bytes, SHA-256 `c8a27c81317c19cb699773a8541e3eb8313a4ecfd5f2fe5b6f797ecd3e93a645` | `docs/tsse/evidencias/10_scope_reconciliation/milestone_4_scope_reconciliation_20261015T171524Z/tasks_63_of_67_historical.md` |
| Consolidated Milestone 4 index | Current technical result map and disclosed integrity exception | `docs/tsse/evidencias/milestone_4_manifest.md` |

HEAD alone does not reproduce the formal tests because the test and integration inputs were dirty. Their hash list was captured after execution, not before it, and this temporal limitation must accompany any reproducibility claim.

## Objective 1: Test planning and traceability

Project source: `openspec/changes/tsse-embedded-software-testing/proposal.md`, which calls for a Master Test Plan and bidirectional traceability.

| Evidence | Path |
|---|---|
| Master Test Plan | `docs/tsse/master_test_plan.md` |
| Requirement and testing-objective matrix | `docs/tsse/matriz_trazabilidad.md` |
| Atomic case catalog | `docs/tsse/catalogo_casos.md` |
| Planning review gate | `docs/tsse/evidencias/01_proyecto/milestone_1_revision_plan.md` |
| Initial traceability review | `docs/tsse/evidencias/01_proyecto/milestone_1_revision_trazabilidad.md` |
| Final bidirectional audit | `docs/tsse/evidencias/08_trazabilidad/milestone_4_20261015T161203Z/bidirectional_traceability_audit.md` |
| Raw-to-summary audit | `docs/tsse/evidencias/08_trazabilidad/milestone_4_20261015T161203Z/raw_to_summary_audit.md` |
| Final status inventory | `docs/tsse/evidencias/08_trazabilidad/milestone_4_20261015T161203Z/final_status_inventory.md` |

Result states remain distinct: `PASS`, `FAIL`, `BLOCKED`, `NOT RUN`, and `OUT OF SCOPE`. Mocked evidence is not used to claim physical verification.

## Objective 2: Formal Control Flow Testing

Project source: `openspec/changes/tsse-embedded-software-testing/proposal.md`, which defines a source-grounded CFT model rooted at `acquisition_run_samples()`.

| Artifact | Result/path |
|---|---|
| Model and derivation | `docs/tsse/cft_acquisition_run_samples.md` |
| Canonical graph source | `docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.dot` |
| Existing reusable vector figure | `docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.svg` |
| Existing reusable raster figure | `docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.png` |
| Independent source/model review | `docs/tsse/evidencias/03_cft/milestone_1_revision_cft.md` |
| Selected paths and complete oracles | `docs/tsse/cft_cases.md` |
| CFT case review | `docs/tsse/evidencias/03_cft/milestone_1_revision_casos_cft.md` |
| Machine-readable execution | `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json` |
| JUnit execution | `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_junit_tests_report.xml` |

The canonical model has 75 nodes, 106 edges, 32 binary decisions, and cyclomatic complexity `V(G)=106-75+2=33`. The selected feasible set contains exactly 24 paths, `CFT-P01` through `CFT-P24`; all 24 passed. This is component-depth coverage of acquisition-local flow, not exhaustive driver, HAL, hardware, timing, or persistence coverage.

Recommended example for later reuse: `CFT-P21`, the two-sample success path with one loop back edge and one wait. Its path and oracle are in `docs/tsse/cft_cases.md`, and its executed result is in the acquisition JSON/JUnit reports above.

## Objective 3: Automated unit and host-integration testing

Project source: `openspec/changes/tsse-embedded-software-testing/proposal.md`, which defines host-native unit tests of unchanged production sources and software-only integration with simulated hardware boundaries.

### Unit and CFT inventory

| Classification | Unique cases | Result | Machine-readable evidence |
|---|---:|---:|---|
| ADS1263 non-CFT unit | 66 | 66 PASS, 0 FAIL | `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/ads1263_tests_report.json` |
| Acquisition non-CFT robustness | 6 | 6 PASS, 0 FAIL | `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_junit_tests_report.xml` |
| Non-CFT unit total | 72 | 72 PASS, 0 FAIL | Derived as 66 + 6 from the reports above |
| Acquisition CFT | 24 | 24 PASS, 0 FAIL | `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/acquisition_tests_report.json` |

The acquisition command executed 30 cases together: 24 CFT plus 6 non-CFT robustness cases. Do not describe all 96 cases as unit tests without preserving this classification.

Successful raw transcripts:

- `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/03_ads1263_clean_test_all_pass.typescript`
- `docs/tsse/evidencias/04_unitarias/milestone_4_20261015T161203Z/04_acquisition_cft_clean_test_all_after_fix.typescript`

### Host integration

| Measure | Run 1 | Run 2 |
|---|---:|---:|
| Declared/executed unique IDs | 71/71 | 71/71 |
| PASS / FAIL / BLOCKED | 71 / 0 / 0 | 71 / 0 / 0 |
| Assertions | 251/251 | 251/251 |

Authoritative summaries:

- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/summary.json`
- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_2/summary.json`

Raw transcripts:

- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/02_run_1.typescript`
- `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/03_run_2.typescript`

These are 71 unique scenarios executed twice, for 142 scenario executions, not 142 distinct cases. The UTC intervals overlap, so describe them as two process invocations/repetitions using the same build and host, not as independent replications.

The native integration build links real `main`, acquisition, ADS1263 core, and logger. Built-in mock scenarios bypass Linux device operations; four fake-HAL scenarios replace Linux SPI/GPIO adapter objects while exercising real protocol/orchestration code. Build provenance is in `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/build_manifest.json` and `01_build.typescript`.

### Historical legacy integration inventory

The legacy catalog has 29 IDs: 19 `PASS` and 10 `NOT RUN`. The `NOT RUN` IDs are:

`INT-ACQ-001`, `INT-ACQ-002`, `INT-CSV-001`, `INT-CSV-002`, `INT-CSV-003`, `INT-LIVE-001`, `INT-LIVE-002`, `CLI-004-D`, `CLI-005`, and `CLI-006-F`.

Source: `docs/tsse/catalogo_casos.md` and `docs/tsse/evidencias/08_trazabilidad/milestone_4_20261015T161203Z/integration_case_traceability.md`. Nearby but non-identical executed inputs do not change those historical states.

## Objective 4: Software-quality analysis and reproducible evidence

Project source: `openspec/changes/tsse-embedded-software-testing/proposal.md`, which defines coverage, static analysis, sanitizer-assisted robustness investigation, defect records, and reproducibility metadata.

### Coverage

| Production module | Lines | Functions | Branches | Authoritative report |
|---|---:|---:|---:|---|
| `ads1263.c` | 151/222 (68.0%) | 13/16 (81.2%) | 90/134 (67.2%) | `docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/ads1263/GcovCoverage.json` |
| `acquisition.c` | 118/168 (70.2%) | 9/13 (69.2%) | 53/76 (69.7%) | `docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/acquisition/GcovCoverage.json` |

No minimum percentage was an acceptance gate. The authoritative acquisition export is the serial rerun `docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/03_acquisition_coverage_serial_pass.typescript`; the earlier parallel export is preserved as an infrastructure failure. The ADS1263 transcript contains six `libgcov profiling error` messages for excluded test-support file `wait_wrapper.gcda`; filtered production counts agree across JSON, HTML, and text, but the run was not warning-free.

### Static analysis

| Configuration/result | Evidence |
|---|---|
| Cppcheck 2.19.0, `ADS1263_NO_GPIOD=1`: completed with findings, not clean | `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/raw_analysis_exhaustive/001_cppcheck_exhaustive_no_gpiod.stderr.txt` |
| 41 style diagnostics: 37 `unusedStructMember`, 1 `constParameterPointer`, 3 `staticFunction` | `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/raw_final/008_exhaustive_diagnostic_counts.stdout.txt` |
| 38 context/tool false positives; 3 accepted public-interface observations | `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/findings.md` |
| 32 `missingIncludeSystem` environment limitations | `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/findings.md` |
| libgpiod-v2 Cppcheck/GCC branch `BLOCKED`: missing `pkg-config` and `gpiod.h` | `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md` |
| GCC 15 no-gpiod informational fallback: zero diagnostics | `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/raw_analysis/003_gcc15_no_gpiod.stderr.txt` |

Cppcheck exit `2` reflects the configured findings exit policy, not a claim that no analysis completed. No Cppcheck diagnostic was classified as a confirmed production defect.

### Defects, robustness, and unresolved contracts

| ID | Classification | Factual result/evidence |
|---|---|---|
| DEF-001 / DSP-017 | Confirmed defect, open | `rate_hz=1` produces invalid `{0,1000000000}` sleep and an unchanged retry; process reproduction exits 124 by timeout. `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/DEF-001-DSP017-rate-1-livelock.md` |
| ROB-001 / FND-ROB001-001 | Suspected defect, not confirmed | Crossing `start=31,count=2` affects the byte after the register array; ASan/UBSan emit no diagnostic because the access remains within the containing object. `docs/tsse/evidencias/07_defectos_regresion/milestone_3a_20261015T150616Z/README.md` |
| ROB-002 | Expected behavior on executed boundaries; partial | Core/CFT/fake-HAL SPI failure propagation passes; adapter `UT-SPI-004` remains `NOT RUN`, so real `ioctl` behavior is not established. |
| ROB-003 / DSP-011 | Characterized contract ambiguity | DRDY error/timeout followed by SPI success returns OK with diagnostic sample status; no approved contract requires fatal propagation. |

Other open decisions are listed in `docs/tsse/discrepancias.md` and `docs/tsse/matriz_trazabilidad.md`: REQ-012/ARCH-001 Linux isolation, CSV append versus truncation, CSV-open failure semantics, CSV/check CLI policy, the combined register-range contract, DRDY fatality, and initialization-state behavior.

Task 9.4 is `NOT APPLICABLE — CONDITION NOT TRIGGERED`: no production correction was approved or implemented, so no post-correction regression run exists or is claimed.

## Tool versions

| Tool | Version | Evidence |
|---|---|---|
| GCC / gcov | 15.2.0 / 15.2.0 | `docs/tsse/evidencias/06_cobertura/milestone_4_20261015T161203Z/04_tool_versions_post_run.typescript` |
| gcovr | 7.2 | Same path; post-run capture |
| Ceedling | 1.1.10-85ec551 | Same path; post-run capture |
| Unity / CMock | 2.7.2 / 2.7.2 | Same path; post-run capture |
| Cppcheck | 2.19.0 | `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/README.md` |
| Python / strace | 3.14.4 / 6.19 | `docs/tsse/evidencias/05_integracion_host/milestone_4_20261015T161203Z/run_1/tool_preconditions.json` |
| Graphviz `dot` | 14.1.2 | `docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.svg` |

The coverage tool-version record is explicitly post-run metadata from the same reportedly unchanged environment, not a simultaneous pre-run capture.

## Evidence integrity and checksum limitations

OpenSpec task 13.1 remains incomplete because strict append-only preservation cannot be achieved retroactively:

- Static-analysis `00_policy.md`, `README.md`, and `05_finalize_evidence.sh` were changed after their historical checksum manifest.
- The earlier presentation PPTX binary was replaced at the same path, and `deck.sha256` was updated in place. Superseded hashes remain, but the earlier binaries do not.

Available static provenance:

- Historical manifest: `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/raw_final/011_evidence_sha256.stdout.txt`
- Integrity addendum: `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/07_integrity_addendum.md`
- Corrected complete manifest: `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/integrity_review_20261015T162917Z/final/011_complete_sha256.txt`
- Corrected verification result: `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/integrity_review_20261015T162917Z/final/012_complete_sha256_verify.exitcode.txt`

The old manifest verified before correction and later identifies exactly the three changed static files. The corrected set records 313 inventory entries, 305 checksummed entries, eight explicit self/post-generation exclusions, and verification exit 0.

Prospective policy for future runs: use immutable unique run IDs; refuse existing destinations; checksum inputs and outputs before interpretation edits; never regenerate into an evidence path already published; issue separately checksummed correction addenda instead of changing historical files.

## Hardware context and claim boundary

The project records that the original software had been functionally validated on STM32MP157D-DK1 with ADS1263 (`docs/tsse/master_test_plan.md`). No primary raw log from that historical campaign is present, so it is project context rather than reproducible TSSE evidence.

The current TSSE campaign is host/software-only. It does not verify physical SPI/GPIO, Linux `ioctl` or libgpiod behavior, wiring, electrical behavior, ADC accuracy, target deployment, or real-time timing. Built-in mock and fake HAL results must not be presented as hardware validation.

## Suggested reuse in external academic deliverables

Reuse existing artifacts; do not regenerate them merely for appearance.

| Intended reuse | Existing source |
|---|---|
| CFT figure | `docs/tsse/evidencias/03_cft/acquisition_run_samples_cfg.svg` or `.png` |
| CFT path example | `CFT-P21` in `docs/tsse/cft_cases.md` |
| Test-result table | Unit JSON/JUnit reports and both integration `summary.json` files listed above |
| Coverage table | Both `GcovCoverage.json` files listed above |
| Static finding table | `docs/tsse/evidencias/02_analisis_estatico/milestone_4_20261015T161203Z/raw_final/008_exhaustive_diagnostic_counts.stdout.txt` |
| Defect/robustness table | DEF-001 record and Milestone 3A robustness README listed above |
| Unit log excerpt | Final summary in `04_acquisition_cft_clean_test_all_after_fix.typescript` |
| Integration log excerpt | `SUMMARY: PASS scenarios=71 passed=71 failed=0 blocked=0 declaration_match=True` in each integration transcript |
| Confirmed-defect excerpt | `DSP-017 probe: starting N=2 rate_hz=1` and exit 124 in `03_dsp017_probe_timeout.typescript` |

## Final-review boundary

This reconciliation read and parsed retained evidence only. It did not rerun unit tests, CFT cases, integration scenarios, coverage collection, sanitizers, Cppcheck, or GCC analysis. Commands executed during final reconciliation are limited to OpenSpec status/validation, Git baseline checks, path checks, and checksum verification; they are not new test results.
