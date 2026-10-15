# Tasks

## 1. Repository Audit and Baseline Preservation

- [ ] 1.1 Capture branch, full HEAD, baseline commit, worktree state, OS/architecture, and source-file checksums in a new baseline evidence set; verify it identifies `feature/tsse-testing`, `c59b24017f5daebb4e37512d799a0d07f589bce5`, and all production C/H files.
- [ ] 1.2 Inventory production modules, public functions, build inputs, direct external dependencies, and current test/tooling files; verify the inventory links each selected SUT function to its source location and dependency boundary.
- [ ] 1.3 Create a discrepancy register without overwriting prior documents; verify it includes legacy hardware/cross-build scope, REQ-012 Linux-type leakage, logger/CSV responsibility, CSV truncation/failure behavior, CLI order dependence, ROB-001, and ROB-003.
- [ ] 1.4 Establish a production-baseline guard that reports any diff in `components/**/*.c`, `components/**/*.h`, `main/*.c`, and the production `Makefile`; verify its initial result is empty and retain the command/output as evidence.

## 2. Master Test Plan Review and Finalization

- [ ] 2.1 Extend `docs/tsse/master_test_plan.md` in place with environment, roles, entry/exit criteria, suspension/resumption criteria, risks, defect workflow, evidence policy, and schedule; verify existing scope and priority percentages remain visible and unchanged unless explicitly approved.
- [ ] 2.2 Clarify test levels and techniques, including static analysis as verification rather than dynamic testing, component-depth CFT, unit boundaries, host integration, and hardware exclusions; verify every selected subsystem has a stated technique and objective.
- [ ] 2.3 Resolve the efficiency-priority presentation by defining a limited host-observable indicator or an analysis-only limitation; verify no host timing metric is represented as real-time target performance.
- [ ] 2.4 Conduct a Master Test Plan review gate; verify reviewer disposition, unresolved assumptions, and approval state are recorded before CFT cases or automated tests are finalized.

## 3. Requirement-to-Test Traceability

- [ ] 3.1 Classify every original REQ-001..REQ-012 row as software-contract, partial host/mocked, requires hardware, excluded, or unresolved; verify no mocked case closes a physical requirement without qualification.
- [ ] 3.2 Add testing-infrastructure requirement and robustness-hypothesis rows with stable identifiers; verify ROB-001..ROB-004 remain separate from original application requirements.
- [ ] 3.3 Expand planned test groups into atomic case records containing ID, source requirement/hypothesis, level, technique, preconditions, inputs, oracle, state, evidence path, and defect link; verify required fields are present for every case.
- [ ] 3.4 Standardize traceability states to `PASS`, `FAIL`, `BLOCKED`, `NOT RUN`, and `OUT OF SCOPE`; verify unexecuted in-scope cases remain `NOT RUN` or `BLOCKED`, while deliberately excluded cases use `OUT OF SCOPE` with rationale.

## 4. CFT Control-Flow Model

- [ ] 4.1 Freeze the CFT source revision and boundary at `acquisition_run_samples()` plus acquisition-local helpers, with external contracts for ADS1263, logger, file, and timing services; verify a call-boundary table maps every included/excluded call.
- [ ] 4.2 Produce a numbered CFG, node table, and edge list covering short-circuit validation, preparation stages, CSV branches, fixed loop, sample failure, terminal break, sleep retry, and cleanup; verify every node maps to baseline source lines.
- [ ] 4.3 Document decision counting and cyclomatic-complexity convention, including compound predicates; verify the reported value can be recomputed from the graph under the stated convention.
- [ ] 4.4 Add feasibility constraints and identify infeasible or environmentally unbounded paths; verify zero-iteration fixed success, live branches, invalid-channel configuration, zero-rate sleep, and persistent sleep failure are treated accurately.
- [ ] 4.5 Complete an independent source-to-model review; verify every discrepancy is corrected or recorded and the preliminary diagram is not relabeled final before approval.

## 5. CFT-Derived Test Cases

- [ ] 5.1 Select and document a defensible independent/edge path set with node sequences, predicates, and feasibility rationale; verify the set covers validation rejection, each preparation stage, fixed-loop success, sample failures, CSV outcomes, sleep retry, and cleanup.
- [ ] 5.2 Derive boundary and equivalence cases for samples `{0,1,1000,1001}`, rate `{0,1,10,11}`, and channel `{0,unsupported}`; verify each value has an explicit expected validation or downstream result.
- [ ] 5.3 Define each CFT case with preconditions, inputs, dependency script, action, expected return, logs/outputs, call counts, file effects, and cleanup; verify `N=1` expects no sleep and `N=2` expects one back edge and one sleep.
- [ ] 5.4 Review path-to-case mapping and claim language before implementation; verify every selected path has a case or justified non-execution and no claim extends to ADS1263 internals, HAL, physical hardware, or exhaustive unbounded paths.

## 6. Ceedling Setup

- [ ] 6.1 Verify and record GCC, Ceedling, Unity, CMock, gcov, gcovr, and related runtime versions without installing packages; verify unavailable or mismatched tools are marked `BLOCKED` rather than substituted silently.
- [ ] 6.2 Add an independent Ceedling configuration for C11 native builds with `_POSIX_C_SOURCE=200809L`, production include/source paths, CMock support, and separate normal/coverage/sanitizer output; verify configuration inspection shows no target SDK or physical device dependency.
- [ ] 6.3 Add minimal test-support seams for SPI, GPIO, and waits using generated mocks or test-only wrappers; verify a smoke suite compiles and links the actual `components/ads1263/src/ads1263.c` without Linux adapter objects.
- [ ] 6.4 Document non-interactive clean, unit, coverage, and sanitizer commands and their expected exit semantics; verify the normal unit command runs from repository context and nonzero status is observable on an intentional temporary failing assertion.
- [ ] 6.5 Audit test build provenance and production diffs; verify no copied algorithm, production `UNIT_TEST` branch, static-helper exposure, production C/H modification, or production Makefile modification was introduced.

## 7. Unit Test Implementation

- [ ] 7.1 Implement voltage-conversion cases for zero, signs, representative values, integer extrema, gains, Vrefs, tolerance, and gain-zero characterization; verify all cases execute against `ads1263_raw_to_voltage()` from production source.
- [ ] 7.2 Implement register read/write argument, boundary, frame, payload/response, and transfer-error cases; verify callbacks assert complete RREG/WREG bytes and output-buffer effects for valid ranges.
- [ ] 7.3 Implement presence cases for null arguments, transfer failure, all zero, all `0xFF`, each mandatory baseline mismatch, accepted ID variation, and success; verify snapshots and statuses are asserted for each partition.
- [ ] 7.4 Implement channel cases for null/unsupported input, write failure, read failure, mismatch, START1 failure, and successful ordering; verify later calls are suppressed after each failure.
- [ ] 7.5 Implement sample cases for null arguments, mock determinism, immediate/delayed/no DRDY, GPIO error, timeout, SPI error, exact RDATA1 frame, signed decoding, status/checksum, and voltage; verify timeout cases use no real one-second wait.
- [ ] 7.6 Implement acquisition cases corresponding to the approved CFT set with an ADS1263 mock and logger/event spy; verify returns, ordered calls, sample/sleep counts, CSV effects, and cleanup match each path oracle.
- [ ] 7.7 Run the unit suite twice from clean test build state; verify identical case inventory and outcomes, no `/dev/spidev*` or `/dev/gpiochip*` access, and preserved raw outputs for both runs.

## 8. Controlled Robustness Investigations and Review Gate

- [ ] 8.1 Execute ROB-001 crossing-range read/write cases in an isolated normal profile with canaries or explicit state checks; verify actual status and memory effects are preserved without assuming rejection or changing production code.
- [ ] 8.2 Execute finite ROB-001 cases in a separate AddressSanitizer profile and optional UBSan profile; verify each finding records a stable ID, source location, impact, reproducibility, disposition, defect/risk link, complete diagnostic, exit status, and known intra-object-detection limitations.
- [ ] 8.3 Execute ROB-003 cases for GPIO-read failure and DRDY timeout followed by SPI success/failure; verify function return, SPI activity, sample fields/status, and acquisition consequence are recorded separately.
- [ ] 8.4 Complete ROB-002 SPI-failure and ROB-004 conversion-boundary evidence; verify each hypothesis has an atomic result and traceability link.
- [ ] 8.5 Hold the mandatory production-change review gate; verify characterization is preserved, any claimed defect violates an explicitly approved contract, proposed correction and regression cases receive separate review, and production C/H plus production Makefile diffs remain empty. If a correction is requested, stop and obtain an approved OpenSpec update or separate change before editing production code.

## 9. Test Execution and Regression

- [ ] 9.1 Run the complete normal host unit suite from a clean test build and preserve command, metadata, stdout, stderr, exit code, and case summary; verify traceability states match the raw results.
- [ ] 9.2 Triage every failure without deleting or weakening a case; verify each failure has a test-infrastructure fix, defect/ambiguity record, or `BLOCKED` disposition linked to evidence.
- [ ] 9.3 Rerun affected suites after test-infrastructure corrections and preserve both before/after runs; verify no prior failure evidence is overwritten.
- [ ] 9.4 If a separately approved production correction later occurs, rerun the minimal reproduction, all relevant unit/integration suites, production build check, static analysis, and coverage; verify before/after and regression evidence are linked.

## 10. Coverage Reporting

- [ ] 10.1 Configure a dedicated GCC/gcov coverage profile and gcovr filters for tested production modules while excluding tests, generated runners/mocks, frameworks, and support fakes; verify the report file list and denominator are correct.
- [ ] 10.2 Run the identified passing or qualified suite under coverage with stale profile data removed; verify line, branch, and function reports are generated with matching GCC/gcov versions.
- [ ] 10.3 Analyze uncovered decisions and functions by module and link justified gaps to infeasible, excluded, blocked, or deferred cases; verify no minimum percentage is presented as an acceptance gate.
- [ ] 10.4 Preserve text and human-readable coverage artifacts plus command metadata; verify every percentage cited in traceability, report, or slides resolves to this run and named module scope.

## 11. Host-Based Software Integration Testing

- [ ] 11.1 Define process-level scenarios with real-module boundaries, simulated dependencies, commands, expected outputs, exit statuses, timeouts, cleanup, and limitations; verify each scenario is approved before execution.
- [ ] 11.2 Build a native software-only application variant using unchanged `main`, acquisition, ADS1263, and logger production sources; verify it links without target SDK or physical device access and record all substitutions/defines.
- [ ] 11.3 Execute `--check --mock`, fixed mock samples, successful mock CSV, and unavailable-CSV-path scenarios; verify successful-path creation/truncation, exact header, representative row fields, requested row count, close/cleanup, and failure-path warnings, console continuation, process result, and preserved raw/file evidence.
- [ ] 11.4 Execute CLI grammar and boundary scenarios, including missing/malformed values, unknown options, conflicting mode orders, and CSV/mode combinations; verify actual exit codes and diagnostic fragments are recorded, including discrepancies.
- [ ] 11.5 Execute bounded live mock plus SIGINT only with reliable process control; verify timely clean termination and no child remains, or record `BLOCKED` with the attempted setup and reason.
- [ ] 11.6 Perform the scripted SPI/GPIO integration feasibility gate; if accepted, implement and execute deterministic happy/error scripts with real core/orchestration modules and record added value, otherwise mark it `OUT OF SCOPE` or `BLOCKED` with rationale.

## 12. Static Analysis

- [ ] 12.1 Recheck Cppcheck availability/version and define exact source, include, platform, language, checks, suppressions, and exit policy; verify the command is documented before interpreting output.
- [ ] 12.2 Run Cppcheck over all declared production C/H files for `ADS1263_NO_GPIOD=1` and the libgpiod-v2 branch when supported; verify raw outputs, exit codes, configuration coverage, and any blocked branch are preserved.
- [ ] 12.3 Record stable ID, source location, severity/impact, reproducibility state, disposition, and defect/accepted-risk link for every finding, including false positives and environment limitations; verify no one-line progress output or incomplete run is summarized as “clean.”
- [ ] 12.4 Optionally compile production sources with additional GCC diagnostics in an informational profile; verify findings are kept separate from mandatory Cppcheck results and are not silently promoted to acceptance criteria.

## 13. Evidence Collection and Traceability Closure

- [ ] 13.1 Extend the evidence tree and manifest without deleting or overwriting existing TSSE evidence; verify unique run identifiers prevent overwrite and categories cover baseline, static, CFT, unit, integration, coverage, defects/regression, traceability, and presentation.
- [ ] 13.2 Add reproducibility metadata to every new run set, including revisions, worktree, host, versions, working directory, exact command, timestamps, exit code, and real/mock/simulated classification; verify references use repository-relative paths or documented variables and a sample run can be repeated from its record.
- [ ] 13.3 Audit raw-to-summary links and status labels; verify every `PASS`, `FAIL`, coverage number, or defect claim has source evidence and every missing activity is `BLOCKED`, `NOT RUN`, or `OUT OF SCOPE`.
- [ ] 13.4 Finalize bidirectional traceability from requirements/hypotheses to atomic cases, results, evidence, and defects; verify every case links back to a requirement or robustness objective and every in-scope requirement has a disposition.
- [ ] 13.5 Audit each confirmed defect record for stable ID, approved contract reference, preconditions, minimal reproduction, expected/actual behavior, impact, evidence, disposition, and regression links; verify unresolved hypotheses and ambiguities are not recorded as confirmed defects.

## 14. Final Technical Report

- [ ] 14.1 Write the report covering the real embedded system, architecture, historical validation, TSSE scope, Master Test Plan, formal CFT derivation, test architecture, implemented cases, actual results, coverage, findings, and limitations; verify each quantitative claim links to evidence.
- [ ] 14.2 Include an explicit discrepancy and uncertainty section for hardware-only requirements, REQ-012, CSV semantics, ROB-001, ROB-003, blocked tools/scenarios, and untested behavior; verify no ambiguity is silently converted into compliance or defect status.
- [ ] 14.3 Perform a factual and reproducibility review against source, traceability, and raw evidence; verify planned, executed, blocked, and historical work remain visibly distinct and reviewer approval is recorded.

## 15. Google Slides Presentation Preparation

- [ ] 15.1 Prepare a Google Slides-compatible main deck and speaker outline covering system/baseline, scope, quality strategy, one readable CFT model/path example, unit/mock architecture, actual results, coverage/traceability, and conclusions/limitations; verify no planned result is presented as executed.
- [ ] 15.2 Add backup slides for detailed paths, test inventory, raw excerpts, findings, coverage by module, and defect records; verify the main narrative remains concise while review evidence is accessible.
- [ ] 15.3 Conduct and record a timed rehearsal targeting approximately ten minutes; verify timing, slide ownership, terminology, hardware disclaimer, and likely question responses are captured in presentation evidence.
