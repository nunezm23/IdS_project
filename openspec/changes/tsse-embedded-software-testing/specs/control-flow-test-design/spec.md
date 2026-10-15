# Spec Delta

## Purpose

Defines a formal, source-grounded Control Flow Testing model for fixed acquisition and the rules for deriving feasible system-level cases and defensible coverage claims.

## ADDED Requirements

### Requirement: Bounded acquisition component model
The CFT model SHALL use `acquisition_run_samples()` as its root and include all acquisition-local helpers reached by that entry point. ADS1263, logger, file, and timing services SHALL be modeled as explicit contract outcomes rather than recursively expanded implementations.

#### Scenario: Model boundary review
- **WHEN** the control-flow graph and call-boundary table are reviewed against `acquisition.c`
- **THEN** validation, preparation, CSV opening, fixed-loop, sleep, logging, and cleanup decisions are represented while external services have named outcomes

### Requirement: Depth interpretation
The CFT report SHALL define “depth 1” as component-depth analysis of the acquisition translation unit, not literal inspection of only the top-level function body and not full expansion of driver or Linux HAL internals.

#### Scenario: Depth claim checked
- **WHEN** the report states the selected CFT depth
- **THEN** it explains why acquisition-local helpers are included and why ADS1263 and platform internals are separate verification scopes

### Requirement: Source-verified control-flow model
The CFT artifact SHALL identify numbered nodes, directed edges, compound predicates with short-circuit behavior, call outcomes, resource cleanup, and observables. A second review SHALL compare the model with the preserved production source before cases are finalized.

#### Scenario: Independent model review
- **WHEN** the draft graph, node table, and edge list are compared with the baseline source
- **THEN** every modeled decision maps to a source location and discrepancies are corrected or recorded before derivation

### Requirement: Feasibility constraints
The derivation SHALL distinguish feasible, infeasible, and environmentally unbounded paths using actual constraints: samples `1..1000`, rate `1..10`, channel `0`, fixed loop mode, short-circuit pointer checks, and reachable dependency outcomes.

#### Scenario: Infeasible zero-iteration path
- **WHEN** a path assumes successful validation followed by zero fixed-loop iterations
- **THEN** the path is rejected because valid `samples` is at least one

#### Scenario: Infeasible live branch
- **WHEN** a path enters the live-loop condition from `acquisition_run_samples()`
- **THEN** the path is rejected because that entry point passes fixed mode to the loop

### Requirement: Loop bounding policy
The CFT design SHALL cover fixed-loop entry, one iteration, a back edge, normal terminal break, first-read failure, later-read failure, and one interrupted sleep followed by success. It SHALL not claim exhaustive paths through unbounded environmental retries.

#### Scenario: Fixed loop representatives
- **WHEN** loop cases are derived
- **THEN** cases include `N=1`, `N=2`, and controlled early failures with expected read, output, sleep, and cleanup counts

### Requirement: Path derivation record
Each selected CFT path SHALL list its node or edge sequence, path predicate, feasibility rationale, preconditions, inputs, controlled dependency outcomes, actions, expected return, expected outputs, and cleanup events.

#### Scenario: Derived preparation failure path
- **WHEN** a presence or channel-configuration failure path is documented
- **THEN** it identifies suppressed later calls, expected diagnostics, ADC cleanup, and return `-1`

#### Scenario: Derived success path
- **WHEN** a fixed acquisition success path is documented for `N` samples
- **THEN** it expects `N` reads and sample outputs, `N-1` sleeps, final cleanup, and return `0`

### Requirement: Complementary boundary design
CFT cases SHALL be supplemented with boundary-value and equivalence-partition cases for samples, rate, and channel because basis paths alone do not establish input-boundary coverage.

#### Scenario: Boundary set review
- **WHEN** input-derived cases are reviewed
- **THEN** samples include `0, 1, 1000, 1001`, rate includes `0, 1, 10, 11`, and channel includes `0` and at least one unsupported value

### Requirement: CFT claim limitation
The CFT report SHALL state coverage only for the modeled acquisition scope and SHALL not claim exhaustive driver, HAL, physical hardware, real-time, or CSV-persistence coverage.

#### Scenario: Results summary prepared
- **WHEN** CFT execution results are summarized
- **THEN** the summary names the model revision and exercised paths and identifies unmodeled or unexecuted areas without an “all paths” claim unless every declared model path is supported
