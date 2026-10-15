# Spec Delta

## Purpose

Defines an auditable test-planning and traceability contract that relates original product requirements, host-test requirements, atomic test cases, evidence, defects, and explicit verification limitations.

## ADDED Requirements

### Requirement: Baseline and scope preservation
The test plan SHALL identify production commit `c59b24017f5daebb4e37512d799a0d07f589bce5`, branch `feature/tsse-testing`, the historically validated hardware context, and the host-only TSSE execution scope. It SHALL state that host evidence does not verify physical hardware.

#### Scenario: Plan scope review
- **WHEN** the finalized Master Test Plan is reviewed
- **THEN** it identifies the commit, branch, host environment, included software, excluded hardware activities, and the limitation on physical verification

### Requirement: Existing priority decisions
The test plan SHALL preserve the approved quality and subsystem priority percentages unless a revision is explicitly identified, justified, and approved rather than silently substituted.

#### Scenario: Priority consistency check
- **WHEN** plan priorities are compared with `docs/tsse/master_test_plan.md`
- **THEN** the existing percentages match or each difference has a recorded rationale and approval state

### Requirement: Requirement classification
Traceability SHALL distinguish original application requirements, testing-infrastructure requirements, and robustness hypotheses. A host test SHALL qualify its verification claim as software-contract, partial host/mocked, excluded, blocked, or requiring hardware as applicable.

#### Scenario: Physical-facing requirement mapped to a mock
- **WHEN** a mocked test is linked to SPI, GPIO, target, or ADC behavior
- **THEN** the traceability entry limits the claim to the exercised software contract and does not mark physical behavior as verified

#### Scenario: Robustness hypothesis recorded
- **WHEN** ROB-001 or ROB-003 is added to the case inventory
- **THEN** it remains classified separately from original requirements and is not labeled a confirmed defect before controlled evidence exists

### Requirement: Atomic traceability records
Each executable or analytical case SHALL have a stable identifier and links to its source requirement or hypothesis, test level, technique, preconditions, inputs, expected result, execution state, evidence path, and defect identifier when applicable.

#### Scenario: Traceability completeness review
- **WHEN** an implemented case is selected from the matrix
- **THEN** every required traceability field is populated and its referenced evidence or explicit non-execution reason can be located

### Requirement: Standard result states
Test reporting SHALL use `PASS`, `FAIL`, `BLOCKED`, `NOT RUN`, and `OUT OF SCOPE` consistently and SHALL not represent planned or blocked work as executed.

#### Scenario: Planned case before execution
- **WHEN** a case has been designed but no execution evidence exists
- **THEN** its state is `NOT RUN` or `BLOCKED`, not `PASS`

### Requirement: Documentation discrepancy register
The planning record SHALL explicitly track source/documentation tensions that affect test expectations, including TSSE versus legacy acceptance scope, REQ-012 Linux isolation wording, requested CSV failure semantics, DRDY error propagation, and any newly discovered conflict.

#### Scenario: Ambiguous contract affects an oracle
- **WHEN** a test designer cannot derive one normative expected result from approved documentation
- **THEN** the case is marked as characterization or blocked pending a contract decision and the discrepancy is linked

### Requirement: Production modification review gate
No production C/H modification or defect correction SHALL begin until characterization evidence is preserved, the intended contract is approved, the proposed change is separately reviewed, and regression cases are identified.

#### Scenario: Suspected defect reproduced
- **WHEN** a controlled test demonstrates behavior inconsistent with an approved contract
- **THEN** a defect record and pre-correction evidence are created before any production change is authorized

#### Scenario: Planning phase completion
- **WHEN** this OpenSpec proposal workflow finishes
- **THEN** only files under the change directory have been created or updated and no implementation activity has started
