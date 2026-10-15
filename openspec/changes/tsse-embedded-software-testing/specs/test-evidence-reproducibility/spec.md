# Spec Delta

## Purpose

Defines how test commands, environments, raw outputs, summaries, defects, traceability, technical handoff material, and historical academic artifacts are preserved so results remain honest, reviewable, and reproducible.

## ADDED Requirements

### Requirement: Reproducibility metadata
Every execution evidence set SHALL record source and test revisions, worktree state, OS and architecture, tool versions, working directory, exact command, configuration, timestamps, exit code, and whether dependencies were real, mocked, or simulated.

#### Scenario: Independent reviewer reproduces a run
- **WHEN** a reviewer selects a recorded test, coverage, sanitizer, or static-analysis run
- **THEN** the evidence supplies enough metadata and commands to repeat it on a prepared equivalent host

### Requirement: Raw evidence preservation
Raw stdout, stderr, reports, and generated artifacts SHALL be preserved without replacing prior failure evidence. Human-readable summaries SHALL link to raw artifacts and distinguish interpretation from captured output.

#### Scenario: Failure later passes
- **WHEN** a failed case passes after an approved correction
- **THEN** both executions remain available under distinct identifiers or timestamps and the summary links to each

### Requirement: Planned, executed, and blocked separation
Technical evidence and handoff material SHALL clearly distinguish proposed cases, implemented but unexecuted cases, actual executions, blocked work, and historical hardware validation. Any later external academic reuse SHALL retain those distinctions.

#### Scenario: Handoff prepared before a case runs
- **WHEN** the technical handoff mentions that case
- **THEN** it is labeled planned or not run and contains no fabricated output, result, screenshot, or coverage value

### Requirement: Evidence organization
Evidence SHALL be organized by baseline, static analysis, CFT, unit tests, host integration, coverage, defects/regression, traceability, technical handoff, and preserved historical academic artifacts, with a manifest or equivalent index linking stable test IDs to artifacts.

#### Scenario: Evidence audit
- **WHEN** the evidence tree and manifest are reviewed
- **THEN** each completed activity has a discoverable raw artifact and each missing activity has an explicit status

### Requirement: Evidence naming and portability
Generated evidence names SHALL avoid accidental overwrite and SHALL use repository-relative references or documented variables where practical instead of relying on one developer’s absolute paths.

#### Scenario: Evidence generated twice
- **WHEN** two executions of the same suite are preserved
- **THEN** both can be distinguished and links remain valid from the repository context

### Requirement: Defect report content
A confirmed defect record SHALL contain a stable identifier, contract reference, preconditions, minimal reproduction, expected and actual behavior, impact, evidence, disposition, and regression links. A hypothesis SHALL remain labeled as such until these criteria are met.

#### Scenario: ROB-001 produces a sanitizer diagnostic
- **WHEN** controlled evidence demonstrates unsafe register access against an approved range contract
- **THEN** the defect record links the diagnostic and reproduction while preserving the pre-correction source baseline

#### Scenario: ROB-003 lacks an approved return contract
- **WHEN** current DRDY behavior is reproduced but no requirement defines propagation
- **THEN** the record remains a characterized ambiguity rather than a confirmed defect

### Requirement: Technical evidence handoff
The implementation workflow SHALL provide a concise factual index linking the academic TSSE objectives to retained planning, CFT, unit, integration, coverage, static-analysis, defect, traceability, checksum, and reproducibility evidence.

#### Scenario: Handoff readiness review
- **WHEN** the technical evidence handoff is declared complete
- **THEN** every quantitative or defect claim links to execution evidence and every unexecuted, blocked, ambiguous, or hardware-only area is visibly qualified

### Requirement: External academic deliverable ownership
The final academic LaTeX report and final Google Slides presentation SHALL be authored and reviewed outside this OpenCode/OpenSpec implementation workflow. Previously generated report and presentation artifacts SHALL remain historical material and SHALL NOT be deleted, overwritten, regenerated, or represented as newly approved during scope reconciliation.

#### Scenario: Implementation scope is reconciled
- **WHEN** active implementation tasks are reviewed after academic ownership is reassigned
- **THEN** report and presentation authoring tasks are absent from the active checklist, their original checklist text remains in a checksummed historical snapshot, and the task-count change is identified as scope reassignment rather than lost work

### Requirement: Environment change transparency
Missing tools or incompatible versions SHALL be reported as environment constraints. Results SHALL not be fabricated or borrowed, and package installation or environment changes SHALL occur only when separately authorized.

#### Scenario: Required tool is unavailable
- **WHEN** an authorized implementation task reaches an unavailable tool
- **THEN** the task is marked `BLOCKED`, the missing dependency is recorded, and no substitute result is claimed without approval
