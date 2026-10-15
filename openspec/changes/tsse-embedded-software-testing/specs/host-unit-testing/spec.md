# Spec Delta

## Purpose

Defines repeatable native unit testing of the existing embedded C software with controlled dependencies, explicit case design, and no requirement for target hardware or rewritten algorithms.

## ADDED Requirements

### Requirement: Native host test entry point
The project SHALL provide a documented non-interactive unit-test command for Ubuntu 26.04 x86-64 using GCC 15.2 and Ceedling 1.1.10 with Unity and CMock. The command SHALL return nonzero when a test fails.

#### Scenario: Unit suite invoked on a prepared host
- **WHEN** the documented unit-test command is run with the approved tool versions
- **THEN** it compiles the selected suites, reports atomic results, and returns an exit status consistent with the suite outcome

### Requirement: Actual production source under test
Unit suites SHALL compile the selected production `.c` translation units directly and SHALL not use copied, rewritten, or testing-only versions of production algorithms.

#### Scenario: Source provenance audit
- **WHEN** the test build inputs are inspected
- **THEN** each unit under test resolves to its production source path and no duplicate algorithm implementation exists in test support

### Requirement: No production-only testing branches
Dependency substitution SHALL use existing interfaces, generated mocks, link wrappers, test-only headers, or test-support objects without adding `UNIT_TEST` branches or exposing static internals in production C/H files.

#### Scenario: Isolation implementation review
- **WHEN** mocks, fakes, wrappers, and include paths are audited
- **THEN** all testing-specific behavior is outside production C/H files and public functions exercise static helpers indirectly

### Requirement: ADS1263 core isolation
Core driver tests SHALL replace Linux SPI and GPIO operations with controlled doubles and SHALL replace or intercept real waits for timeout cases. Transfer doubles SHALL permit verification of transmitted bytes and injection of received bytes and status codes.

#### Scenario: Register transfer test
- **WHEN** a register operation is executed in non-mock mode under test
- **THEN** the test can assert command, count, length, payload or response offset, and propagated transfer status without opening a device

#### Scenario: DRDY timeout test
- **WHEN** DRDY remains busy for the modeled timeout
- **THEN** the test completes without a real one-second delay and records the expected poll and wait behavior

### Requirement: Selected unit scope
Atomic cases SHALL cover voltage conversion, register reads and writes, presence checking, channel configuration, and sample reading. Each function SHALL include valid behavior, invalid arguments, relevant boundaries, and dependency-error paths.

#### Scenario: Unit inventory review
- **WHEN** the implemented test inventory is compared with the selected scope
- **THEN** every selected public function has linked cases for its planned partitions and error paths or an explicit blocked rationale

### Requirement: Register access design
Register cases SHALL verify RREG/WREG frame construction, first and last valid register access, count boundaries, transfer failures, and combinations whose requested range crosses register index 31.

#### Scenario: Valid final register
- **WHEN** start is 31 and count is 1
- **THEN** the expected one-register operation is asserted

#### Scenario: Cross-boundary hypothesis
- **WHEN** start is 31 and count is 2 in a controlled robustness run
- **THEN** the observed status and memory-safety evidence are preserved as ROB-001 without correcting production code or assuming the intended result

### Requirement: Presence and channel design
Presence cases SHALL cover transport failure, all-zero, all-`0xFF`, each mandatory baseline mismatch, accepted ID variation, and valid baseline. Channel cases SHALL cover unsupported channels and each write, readback, mismatch, and START1 outcome in order.

#### Scenario: Accepted ID variation
- **WHEN** ID differs but POWER, INTERFACE, MODE0, and MODE1 match the baseline
- **THEN** the expected presence result is success, consistent with current component design documentation

#### Scenario: Channel readback failure
- **WHEN** INPMUX write succeeds and readback fails or differs
- **THEN** START1 is not sent and the corresponding status is asserted

### Requirement: Sample and conversion design
Sample cases SHALL verify the seven-byte RDATA1 frame, signed big-endian decoding, status/checksum extraction, voltage conversion, SPI failure, DRDY states, and integer boundary values. Conversion cases SHALL define tolerances and include zero, representative signs, extrema, gains, and the current gain-zero behavior as characterization.

#### Scenario: Signed sample decoding
- **WHEN** controlled SPI responses encode zero, positive, and negative boundary values
- **THEN** raw, voltage, status byte, checksum byte, and return status match the approved oracle within the stated numeric tolerance

### Requirement: DRDY contract characterization
ROB-003 cases SHALL separately record the function return, `sample.status`, SPI activity, and acquisition consequence after a DRDY read error or timeout. They SHALL not classify current behavior as defective until an explicit contract is approved.

#### Scenario: DRDY failure followed by SPI success
- **WHEN** DRDY waiting fails and the subsequent transfer succeeds
- **THEN** the case records the current return value and sample status and marks contract compliance as unresolved rather than silently selecting an oracle

### Requirement: Unit test determinism
Unit tests SHALL not depend on physical devices, real GPIO, uncontrolled signals, wall-clock sleeps, test order, or persistent state from a previous case.

#### Scenario: Repeated suite execution
- **WHEN** the unit suite is run twice from clean test build state
- **THEN** it exercises the same cases with no hardware access and produces equivalent pass/fail outcomes
