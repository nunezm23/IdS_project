# Entorno y procedencia - Milestone 3A

## Identificacion

- Run set: `milestone_3a_20261015T150616Z`.
- Rama: `feature/tsse-testing`.
- HEAD de inicio/ejecucion: `1d25a7c2a79422c92ba690d50fe671853b213578`.
- Baseline de produccion protegido: `c59b24017f5daebb4e37512d799a0d07f589bce5`.
- Directorio: `/home/cronos/projects/tsse/IdS_project`.
- OS: Ubuntu 26.04.1 LTS, Linux 7.0.0-38-generic x86-64.
- GCC/gcov: 15.2.0; gcovr: 7.2; Ruby: 3.3.8.
- Ceedling: 1.1.10-85ec551; Unity/CMock: 2.7.2.

Transcripts individuales conservan branch, HEAD, worktree, host, versiones y working directory. `input_checksums.typescript` identifica fuentes reales, tests, wrappers y perfiles usados. La revision de test permanece no versionada durante la ejecucion y se identifica mediante HEAD, estado y SHA-256.

## Configuraciones

- `project.yml`: suite ADS1263 normal; coverage con `test/ceedling/mixins/coverage.yml`.
- `test/ceedling/project_acquisition_robustness.yml`: suite acquisition enfocada en ROB-003/DSP-017.
- `asan.yml` y `ubsan.yml`: perfiles separados, finitos y sin coverage combinado.
- `acquisition_coverage.yml`: coverage separado de acquisition.
- `processed_*.yml`: configuraciones resueltas. Los intentos `dumpconfig_acquisition*.typescript` fallaron por orden incorrecto de `-p`; los transcripts `*_pass.typescript` conservan la invocacion corregida.

## Limite de afirmacion

La evidencia es host/software-only. No verifica ADS fisico, bus, GPIO, target ni tiempo real. El probe DSP-017 usa `acquisition.c` real con fakes deterministas y `nanosleep` host real bajo timeout externo.

## Validacion final

- `final_production_guard_diff.typescript` y `final_production_guard_names.typescript`: diff C/H/Makefile de produccion vacio contra el baseline protegido, exit 0.
- `final_openspec_validate.typescript`: validacion estricta exit 0.
- `final_git_diff_check.typescript`: whitespace check exit 0.
- `final_json_validation.typescript`: JSON de resultados y coverage valido, exit 0.
- `final_git_status.typescript`: estado de trabajo preservado antes del checksum final.
