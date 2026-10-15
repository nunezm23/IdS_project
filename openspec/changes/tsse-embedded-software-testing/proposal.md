# Proposal

## Why

The ADS1263 acquisition application has already been functionally validated on the STM32MP157D-DK1, but the repository does not yet contain reproducible automated software tests, a completed formal Control Flow Testing (CFT) derivation, or trustworthy execution and coverage evidence. This change plans the host-based verification work required for the TSSE assignment while preserving production commit `c59b24017f5daebb4e37512d799a0d07f589bce5` and clearly separating historical hardware validation, planned work, and future executed results.

## What Changes

- Establish a concise Master Test Plan and bidirectional traceability model that distinguish original application requirements from new testing-infrastructure requirements and from robustness hypotheses.
- Define a formal CFT model rooted at `acquisition_run_samples()` and including acquisition-local helper decisions, feasibility constraints, bounded loop treatment, derived paths, and explicit expected observations.
- Plan native x86-64 unit testing of unchanged production sources with Ceedling 1.1.10, Unity, CMock, GCC 15.2, controlled mocks/stubs, and no physical target dependencies.
- Plan host-based integration scenarios using the built-in mock mode and, if feasible, scripted hardware-interface substitutes without claiming physical SPI, GPIO, timing, or ADC validation.
- Define collection and reporting of gcov/gcovr coverage, Cppcheck findings, AddressSanitizer evidence for ROB-001, optional UndefinedBehaviorSanitizer observations, defect records, regression results, and reproducibility metadata.
- Preserve evidence and provide a factual technical handoff suitable for later academic reuse without assigning final report or presentation authoring to this implementation change.
- Investigate ROB-001 register-range handling and ROB-003 DRDY error propagation as hypotheses. No defect will be declared without controlled evidence; ROB-003 also requires an approved software-contract interpretation.
- Add explicit review gates before any production-code modification or defect correction.

The planned deliverables are finalized TSSE planning and traceability documents, a verified CFT model and derived cases, host-test infrastructure and automated tests, raw and summarized execution evidence, module-scoped coverage reports, static-analysis evidence, defect/regression records, and a factual technical handoff index. The final academic report in LaTeX and the final Google Slides presentation are external deliverables owned by the human user and ChatGPT web workflow, not implementation tasks in this change. In this proposal phase only the OpenSpec planning artifacts are created; no tests are implemented or executed.

### Scope

- Primary unit-test scope: `ads1263_raw_to_voltage()`, register read/write, presence checking, channel configuration, and sample reading in `components/ads1263/src/ads1263.c`.
- Formal CFT scope: `acquisition_run_samples()` and the acquisition-local helpers needed to represent its actual control flow.
- Supporting scope: selected acquisition, logger/CSV, CLI, Linux adapter, and module-integration behaviors where they improve traceability or isolation confidence.
- Existing priority decisions remain: functionality 45%, reliability 30%, maintainability 15%, efficiency 10%; ADS1263 45%, acquisition 35%, CLI 10%, logger/CSV 5%, Linux interfaces 5%.

### Non-Goals

- Repeating electrical or functional hardware validation of the STM32MP157D-DK1, ADS1263, potentiometer, physical SPI, or DRDY signal.
- Cross-compilation, target deployment, flashing, SDK qualification, or real-hardware timing characterization.
- Rewriting production algorithms or adding testing-only branches to production code.
- Correcting suspected defects during planning, inventing coverage thresholds, or presenting expected/mock output as physical or executed evidence.
- Authoring or approving the final academic LaTeX report, Google Slides presentation, speaker notes, or rehearsal.

## Capabilities

### New Capabilities

- `test-planning-traceability`: Defines scope, priorities, test levels, test techniques, status vocabulary, and bidirectional links among requirements, cases, evidence, and defects.
- `control-flow-test-design`: Defines the verified acquisition control-flow model, feasible path derivation, bounded loop policy, and system-level cases with explicit oracles.
- `host-unit-testing`: Defines native compilation of unchanged production C sources, dependency isolation, unit-case design, and repeatable Ceedling execution.
- `host-software-integration`: Defines software-only integration scenarios across selected real modules and simulated hardware boundaries.
- `software-quality-analysis`: Defines module-scoped coverage, static analysis, sanitizer-assisted ROB-001 investigation, result interpretation, and regression measurement without an invented pass threshold.
- `test-evidence-reproducibility`: Defines evidence provenance, preservation, execution-state labeling, defect records, factual technical handoff, and reproducible commands.

### Modified Capabilities

None. No existing OpenSpec capabilities are present, and the original application requirements in `docs/requirements.md` are not changed by this planning change.

## Impact

- Future implementation will add test configuration, test/support sources, scripts or documented commands, reports, and evidence; production C/H files remain unchanged unless a separately reviewed defect correction is later approved.
- Unit tests will compile actual production translation units and replace Linux SPI/GPIO, timing, logging, file, or process dependencies only at existing link/header boundaries or in test support code.
- The production Makefile remains the target-oriented build; Ceedling will provide an independent native-host build configuration.
- Constraints include Ubuntu 26.04 x86-64, GCC 15.2, Ceedling 1.1.10 with Unity/CMock, gcov/gcovr, Cppcheck, no package installation as part of this planning work, and no physical equipment requirement.
- Known documentation tensions must be retained and resolved explicitly: the existing acceptance document includes out-of-scope hardware/cross-build activities; `ads1263.h` exposes Linux types despite REQ-012 wording; CSV-open failures are nonfatal in source; and no documented contract currently requires `ads1263_read_sample()` to return a DRDY failure.
- Principal risks are false confidence from built-in mock mode, accidental conversion of observed behavior into normative requirements, nondeterministic real sleeps/signals, unsafe robustness reproduction, tool/version drift, and overstated requirement or coverage claims.
