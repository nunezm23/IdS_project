# Spec Delta

## Purpose

Defines software-only integration checks that exercise selected real production modules together on the host while replacing physical SPI, GPIO, ADC, and target dependencies.

## ADDED Requirements

### Requirement: Integration scenario declaration
Each host integration scenario SHALL identify the real production modules included, substituted boundaries, command or entry point, preconditions, expected output, exit status, cleanup, and verification limitation.

#### Scenario: Scenario definition review
- **WHEN** an integration scenario is approved for execution
- **THEN** its module boundary and all simulated dependencies are explicit and its oracle can be reproduced without hardware

### Requirement: Built-in mock application flow
The integration suite SHALL exercise the real CLI, acquisition, ADS1263 core, and logger together using built-in mock mode for presence checking and fixed acquisition.

#### Scenario: Mock presence check
- **WHEN** the host application runs `--check --mock`
- **THEN** it exits successfully and reports the simulated baseline without accessing SPI or GPIO devices

#### Scenario: Mock fixed samples
- **WHEN** the host application requests `N` valid samples with `--mock`
- **THEN** it exits successfully and emits exactly `N` deterministic sample records

### Requirement: CSV integration behavior
Host integration SHALL verify CSV creation/truncation, exact header, row count, representative row fields, and cleanup for a successful fixed mock acquisition. CSV-open failure SHALL be characterized separately because current source treats it as nonfatal.

#### Scenario: Successful CSV output
- **WHEN** a fixed mock acquisition writes to a controlled temporary path
- **THEN** the file contains `sample,raw,voltage,status` followed by exactly the requested number of data rows

#### Scenario: CSV path cannot be opened
- **WHEN** a requested path is intentionally unavailable
- **THEN** actual warnings, console continuation, and process result are preserved and contract status is not overstated

### Requirement: CLI boundary integration
Process-level host checks SHALL cover no mode, help, valid modes, invalid numeric boundaries, malformed or missing values, unknown options, repeated or conflicting modes, and unsupported CSV/mode combinations.

#### Scenario: Invalid numeric input
- **WHEN** samples, rate, channel, or DRDY line is outside its accepted CLI range
- **THEN** the process exits nonzero and reports the option and accepted range

#### Scenario: Conflicting modes
- **WHEN** more than one command mode is supplied in each relevant argument order
- **THEN** the observed exit status and diagnostics are compared with the exactly-one-mode contract and any order dependence is recorded

### Requirement: Controlled live-mode integration
If live mode is executed, the scenario SHALL use a bounded process timeout and controlled SIGINT delivery, preserve output and exit status, and guarantee child-process cleanup.

#### Scenario: Live mock termination
- **WHEN** live mock mode has emitted the planned minimum records and receives SIGINT
- **THEN** it terminates within the stated timeout, closes resources, reports termination, and leaves no running child process

### Requirement: Scripted interface integration feasibility gate
The project SHALL perform a feasibility review before deciding whether to add a scripted fake SPI/GPIO integration layer. The layer MAY be added only if it provides meaningful coverage beyond unit tests without production changes or disproportionate schedule risk.

#### Scenario: Scripted interface accepted
- **WHEN** feasibility is confirmed
- **THEN** the scenario exercises real protocol and orchestration modules with deterministic register, DRDY, sample, and error scripts and records its added coverage

#### Scenario: Scripted interface deferred
- **WHEN** feasibility, dependencies, or schedule are inadequate
- **THEN** the task is marked `BLOCKED` or `OUT OF SCOPE` with rationale and the mandatory built-in mock scenarios remain unaffected

### Requirement: Integration claim limitation
Host integration results SHALL be labeled software-only and SHALL not be used to claim target execution, electrical correctness, physical SPI/GPIO behavior, ADC accuracy, or real-time sampling performance.

#### Scenario: Integration summary published
- **WHEN** integration results are included in the technical handoff or reused in an external academic deliverable
- **THEN** each result identifies simulated boundaries and states that physical hardware was not verified
