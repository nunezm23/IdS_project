# Spec Delta

## Purpose

Defines trustworthy measurement and interpretation of code coverage, static-analysis findings, sanitizer observations, and regression outcomes for the software actually exercised on the host.

## ADDED Requirements

### Requirement: Module-scoped coverage reporting
Coverage reporting SHALL identify the exact production modules, test suites, compiler profile, and exclusions used to calculate line, branch, and function metrics. Generated runners, mocks, framework code, tests, and support fakes SHALL not inflate production coverage.

#### Scenario: Coverage report generated
- **WHEN** instrumented tests complete
- **THEN** gcov/gcovr artifacts report line, branch, and function metrics for named production files and list the applied exclusions

### Requirement: No invented coverage threshold
The initial coverage baseline SHALL report measured values and uncovered behavior without applying an unapproved minimum percentage or retroactively redefining acceptance.

#### Scenario: Initial baseline below an unstated expectation
- **WHEN** measured coverage is lower than a stakeholder informally expected
- **THEN** the actual result remains reportable and any future threshold is proposed and approved prospectively

### Requirement: Coverage result integrity
Coverage SHALL be generated only from a completed, identified test run using compatible compiler and gcov versions. Failed or blocked tests and stale profile-data handling SHALL be disclosed with the report.

#### Scenario: Coverage evidence review
- **WHEN** a coverage percentage is cited
- **THEN** its source report, production-file denominator, test run, tool versions, and suite outcome are traceable

### Requirement: Reproducible static analysis
Static analysis SHALL record the exact Cppcheck version, command, configuration defines, include paths, source scope, exit code, raw output, and finding summary. Missing dependencies or incomplete runs SHALL be reported as blocked or incomplete, not clean.

#### Scenario: Cppcheck completes
- **WHEN** Cppcheck analyzes the declared production scope
- **THEN** raw output and a count by severity with dispositions are preserved alongside command and exit status

#### Scenario: Cppcheck cannot analyze a branch
- **WHEN** required headers or tool support are unavailable
- **THEN** the affected configuration is identified as blocked and no zero-finding claim is made

### Requirement: Configuration-aware static scope
Static analysis SHALL account for both the Linux GPIO implementation and `ADS1263_NO_GPIOD` fallback when the environment can support them, or SHALL record which configuration could not be analyzed and why.

#### Scenario: Conditional GPIO code reviewed
- **WHEN** static-analysis results are summarized
- **THEN** the analyzed preprocessor configurations and any omitted branch are explicit

### Requirement: Controlled sanitizer investigations
AddressSanitizer and optional UndefinedBehaviorSanitizer runs SHALL use separate build profiles from coverage and SHALL target finite host cases where they add diagnostic value, especially ROB-001. Sanitizer output SHALL be treated as evidence, not silently suppressed.

#### Scenario: Register range investigation
- **WHEN** a cross-boundary mock register case is executed under a sanitizer
- **THEN** the command, exit status, full diagnostic, and limitations of the sanitizer’s object-bound detection are preserved

### Requirement: Finding disposition
Each static-analysis or sanitizer finding SHALL receive a stable identifier, affected source location, severity or impact, reproducibility state, disposition, and link to a defect or accepted-risk record.

#### Scenario: Potential finding is not reproducible
- **WHEN** analysis reports a condition that controlled testing cannot confirm
- **THEN** it is retained with an unresolved or false-positive rationale rather than counted as a confirmed production defect

### Requirement: Regression measurement
After any separately approved correction, the affected reproduction case, relevant unit and integration suites, and coverage/static checks SHALL be rerun and linked to before-and-after evidence.

#### Scenario: Production correction approved later
- **WHEN** a confirmed defect is corrected after the review gate
- **THEN** the original failure, corrected pass, regression results, and metric changes are traceable without overwriting prior evidence
