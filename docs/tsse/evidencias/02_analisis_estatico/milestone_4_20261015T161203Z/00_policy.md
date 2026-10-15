# Static-analysis execution policy

Run ID: `milestone_4_20261015T161203Z`

This policy was written before analyzer output was produced or interpreted. The run is append-only: collectors refuse to replace an existing artifact. All commands run from the repository root. Dependencies are real host tools and headers; no packages are installed and no hardware is accessed. The evidence process is scoped not to edit production, build, test, OpenSpec, historical-evidence, or other documentation paths; worktree snapshots are observations and cannot establish authorship of concurrent changes outside this run directory.

## Declared production scope

Exactly these six C and six H files are in mandatory Cppcheck scope:

```text
main/main.c
components/logger/src/app_log.c
components/acquisition/src/acquisition.c
components/ads1263/src/ads1263.c
components/ads1263/arch/linux/src/ads1263_spi_linux.c
components/ads1263/arch/linux/src/ads1263_gpio_linux.c
components/logger/include/app_log.h
components/acquisition/include/acquisition.h
components/ads1263/include/ads1263.h
components/ads1263/include/ads1263_defs.h
components/ads1263/arch/linux/include/ads1263_spi_linux.h
components/ads1263/arch/linux/include/ads1263_gpio_linux.h
```

Explicit project include paths:

```text
-Icomponents/logger/include
-Icomponents/acquisition/include
-Icomponents/ads1263/include
-Icomponents/ads1263/arch/linux/include
```

## Mandatory Cppcheck policy

- Language and standard: `--language=c --std=c11`.
- Platform and library model: `--platform=unix64 --library=posix`.
- Checks: `--enable=all --inconclusive --force`.
- POSIX feature selection: `-D_POSIX_C_SOURCE=200809L`.
- Finding exit status: `--error-exitcode=2`; exit 0 means the configured run completed without a diagnostic that activates the error exit, exit 2 means it completed with at least one such diagnostic, and any other nonzero status is investigated as a possible invocation/tool failure. Exit status alone is never interpreted as proof of completeness or cleanliness.
- Suppressions: none. This avoids concealing diagnostics. Environment/tool informational diagnostics are retained and dispositioned rather than suppressed.
- Configuration 1: `-DADS1263_NO_GPIOD=1`; mandatory and runnable independently of libgpiod.
- Configuration 2: `-DADS1263_NO_GPIOD=0`; runnable only if both `pkg-config --exists 'libgpiod >= 2.0'` and a compiler syntax probe of `#include <gpiod.h>` using the package CFLAGS succeed. Otherwise only this configuration is `BLOCKED`, with both gate outputs retained.

The exact no-gpiod command is:

```sh
cppcheck --language=c --std=c11 --platform=unix64 --library=posix --enable=all --inconclusive --force --error-exitcode=2 -D_POSIX_C_SOURCE=200809L -DADS1263_NO_GPIOD=1 -Icomponents/logger/include -Icomponents/acquisition/include -Icomponents/ads1263/include -Icomponents/ads1263/arch/linux/include main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/logger/include/app_log.h components/acquisition/include/acquisition.h components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h
```

The exact libgpiod-v2 command, if its gate passes, is:

```sh
cppcheck --language=c --std=c11 --platform=unix64 --library=posix --enable=all --inconclusive --force --error-exitcode=2 -D_POSIX_C_SOURCE=200809L -DADS1263_NO_GPIOD=0 $(pkg-config --cflags libgpiod) -Icomponents/logger/include -Icomponents/acquisition/include -Icomponents/ads1263/include -Icomponents/ads1263/arch/linux/include main/main.c components/logger/src/app_log.c components/acquisition/src/acquisition.c components/ads1263/src/ads1263.c components/ads1263/arch/linux/src/ads1263_spi_linux.c components/ads1263/arch/linux/src/ads1263_gpio_linux.c components/logger/include/app_log.h components/acquisition/include/acquisition.h components/ads1263/include/ads1263.h components/ads1263/include/ads1263_defs.h components/ads1263/arch/linux/include/ads1263_spi_linux.h components/ads1263/arch/linux/include/ads1263_gpio_linux.h
```

## Informational GCC 15 policy

GCC diagnostics are optional, informational, separate from Cppcheck, and never an acceptance gate. They use the six C translation units, C11, `_POSIX_C_SOURCE=200809L`, explicit project includes, `-fsyntax-only`, `-fanalyzer`, and strong warnings. `-Werror` is prohibited. The no-gpiod configuration runs only if `gcc-15` is available; the libgpiod configuration additionally requires the same v2/header gate as Cppcheck.

Warning profile:

```text
-Wall -Wextra -Wpedantic -Wconversion -Wsign-conversion -Wshadow
-Wformat=2 -Wundef -Wnull-dereference -Wdouble-promotion -Wcast-align
-Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wvla
-fanalyzer
```

## Interpretation and finding policy

- Raw stdout, stderr, exact command, UTC start/end timestamps, and exit code are retained separately for probes and analyzer invocations executed through the collectors' `record()` function. Commands blocked before invocation instead retain `command_not_run`, reason, and status records. The original finalizer's built-in count and inventory steps did not uniformly retain start/end timestamps; this historical metadata limitation is corrected in `07_integrity_addendum.md` and in the reusable finalizer without altering old raw files.
- A run is interpreted only after metadata, HEAD/worktree state, versions, source inventory, checksums, and configuration gates have been captured.
- `PASS` means the declared invocation completed and yielded no actual source finding after review. It does not mean the software is defect-free.
- `FAIL` means a completed invocation produced one or more actual source findings; tool findings require disposition and are not automatically confirmed defects.
- `BLOCKED` applies only to a configuration whose prerequisite tool/header gate failed. A blocked or incomplete configuration is never called clean.
- Every actual finding receives a stable `SA-CPPCHECK-NNN` or `SA-GCC15-NNN` ID, source location, tool severity or assessed impact, reproduction command/artifact, reproducibility state, disposition, and defect/accepted-risk link. Environment limitations receive stable `SA-ENV-NNN` IDs.
- Cppcheck and GCC counts and conclusions remain separate. Progress lines, checker summaries, missing-system-include notices, and tool/environment limitations are not silently counted as production defects, but remain visible and dispositioned.

## Task eligibility rule

OpenSpec tasks 12.1-12.4 are evidence-eligible when their stated behavior is demonstrated by this run set. Their checkboxes remain untouched because this execution is explicitly restricted to files in this directory; completion is reported in `README.md` rather than written to `openspec/.../tasks.md`.
