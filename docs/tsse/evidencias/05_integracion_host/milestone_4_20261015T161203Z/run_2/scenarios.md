# Approved host integration scenarios

Approval-Basis: Current user authorization in the review-remediation request for OpenSpec tasks 11.1-11.6
Approval-Reviewer: Requesting user
Approval-Disposition: APPROVED
Approval-Date: 2026-10-15
Approval-Scope: all declared scenario IDs

All 71 rows below are **APPROVED** for automated execution. `$APP` is the native
binary containing the unchanged Linux adapters; `$FAKE` is the alternate binary
containing the scripted test-only adapter; `$CASE` is the per-case evidence
directory supplied by the runner.

## Boundaries

| Profile | Real production modules | Simulated/substituted boundary | Limitation |
|---|---|---|---|
| M | `main`, acquisition, ADS1263 core, logger, Linux SPI/GPIO adapters | ADS1263 built-in `--mock` register/sample state | Bypasses adapter calls, protocol transfers, GPIO polling, and hardware |
| P | CLI in real `main` and real logger; all M modules remain linked | Invalid/help input prevents acquisition or built-in mock is selected for accepted commands | Establishes parser/process behavior only |
| T | Same as M under `strace` | Built-in mock plus syscall observation | Absence of device access on this run is not adapter/hardware verification |
| L | Same as M in live mode | Built-in mock, PTY, process group, controlled SIGINT | Host scheduling and rate are not real-time evidence |
| H | `main`, acquisition, ADS1263 core, logger | Scripted SPI/GPIO functions replace both Linux adapter objects | Verifies software protocol/orchestration only, not Linux adapters or hardware |

Default timeout is 3 seconds. `INT-STRACE-NO-DEVICE` uses 5 seconds.
`INT-CSV-CLOSE-STRACE` also uses 5 seconds and correlates the CSV `openat`
resulting descriptor with its later successful `close` syscall.
`INT-LIVE-SIGINT` allows 3 seconds to observe two records and 2 seconds after
SIGINT. Every ordinary/trace/HAL process starts in a new process group and is
killed/reaped on timeout. CSV artifacts remain under `$CASE`; fake-HAL resources
must end with `GPIO_CLOSE`, `SPI_CLOSE`; live mode must leave no process group.
These cleanup rules and the applicable profile limitation are part of every row.

## Scenario declarations

| ID | Profile | Command/entry point | Expected output and exit |
|---|---|---|---|
| `INT-MOCK-CHECK` | M | `$APP --check --mock` | Baseline ID `0x23`, detected and DRDY messages; exit 0 |
| `INT-MOCK-SAMPLES` | M | `$APP --samples 3 --channel 0 --rate-hz 10 --mock` | Exactly three deterministic sample records; exit 0 |
| `INT-CSV-CREATE` | M | `$APP --samples 2 --channel 0 --rate-hz 10 --csv $CASE/created.csv --mock` | Exact header, two rows, representative first row, console output; exit 0 |
| `INT-CSV-TRUNCATE` | M | Preseed CSV, then `$APP --samples 1 ... --csv $CASE/truncated.csv --mock` | Stale bytes absent, header plus one row; exit 0 |
| `INT-CSV-FAIL` | M | `$APP --samples 2 ... --csv $CASE/not_a_directory/output.csv --mock` | Both warnings and two console records, no CSV; characterized nonfatal exit 0 |
| `CLI-NO-MODE` | P | `$APP` | Usage; exit 1 |
| `CLI-HELP` | P | `$APP --help` | Usage; exit 0 |
| `CLI-UNKNOWN` | P | `$APP --check --mock --unknown` | Unknown-option error; exit 1 |
| `CLI-MISSING-SAMPLES` | P | `$APP --samples` | Requires-value error; exit 1 |
| `CLI-MISSING-CHANNEL` | P | `$APP --check --mock --channel` | Requires-value error; exit 1 |
| `CLI-MISSING-RATE` | P | `$APP --check --mock --rate-hz` | Requires-value error; exit 1 |
| `CLI-MISSING-DRDY` | P | `$APP --check --mock --drdy-line` | Requires-value error; exit 1 |
| `CLI-MISSING-CSV` | P | `$APP --check --mock --csv` | Requires-value error; exit 1 |
| `CLI-MISSING-SPI` | P | `$APP --check --mock --spi-dev` | Requires-value error; exit 1 |
| `CLI-MISSING-GPIO` | P | `$APP --check --mock --gpio-chip` | Requires-value error; exit 1 |
| `CLI-MALFORMED-SAMPLES` | P | `$APP --samples abc --mock` | Invalid samples error; exit 1 |
| `CLI-MALFORMED-RATE` | P | `$APP --samples 1 --rate-hz 1x --mock` | Invalid rate error; exit 1 |
| `CLI-MALFORMED-CHANNEL` | P | `$APP --samples 1 --channel -1 --mock` | Invalid channel error; exit 1 |
| `CLI-MALFORMED-DRDY` | P | `$APP --check --drdy-line 3.5 --mock` | Invalid DRDY error; exit 1 |
| `CLI-EMPTY-SAMPLES` | P | `$APP --samples '' --mock` | Requires-value error from empty numeric text; exit 1 |
| `CLI-EMPTY-RATE` | P | `$APP --samples 1 --rate-hz '' --mock` | Requires-value error from empty numeric text; exit 1 |
| `CLI-EMPTY-CHANNEL` | P | `$APP --samples 1 --channel '' --mock` | Requires-value error from empty numeric text; exit 1 |
| `CLI-EMPTY-DRDY` | P | `$APP --check --drdy-line '' --mock` | Requires-value error from empty numeric text; exit 1 |
| `CLI-OVERFLOW-SAMPLES` | P | `$APP --samples 184467440737095516160 --mock` | Invalid samples error after `strtoul` overflow; exit 1 |
| `CLI-OVERFLOW-RATE` | P | `$APP --samples 1 --rate-hz 184467440737095516160 --mock` | Invalid rate error after `strtoul` overflow; exit 1 |
| `CLI-OVERFLOW-CHANNEL` | P | `$APP --samples 1 --channel 184467440737095516160 --mock` | Invalid channel error after `strtoul` overflow; exit 1 |
| `CLI-OVERFLOW-DRDY` | P | `$APP --check --drdy-line 184467440737095516160 --mock` | Invalid DRDY error after `strtoul` overflow; exit 1 |
| `CLI-SAMPLES-0` | P | `$APP --samples 0 --mock` | Range `1..1000`; exit 1 |
| `CLI-SAMPLES-1` | M | `$APP --samples 1 --channel 0 --rate-hz 10 --mock` | One sample; exit 0 |
| `CLI-SAMPLES-1000` | P | `$APP --samples 1000 --mock --help` | Accepted maximum reaches help short-circuit, no invalid-samples error; exit 0 |
| `CLI-SAMPLES-1001` | P | `$APP --samples 1001 --mock` | Range `1..1000`; exit 1 |
| `CLI-RATE-0` | P | `$APP --samples 1 --rate-hz 0 --mock` | Range `1..10`; exit 1 |
| `CLI-RATE-1` | M | `$APP --samples 1 --rate-hz 1 --mock` | Configuration says rate 1 and one sample; exit 0 |
| `CLI-RATE-10` | M | `$APP --samples 1 --rate-hz 10 --mock` | Configuration says rate 10 and one sample; exit 0 |
| `CLI-RATE-11` | P | `$APP --samples 1 --rate-hz 11 --mock` | Range `1..10`; exit 1 |
| `CLI-CHANNEL-0` | M | `$APP --samples 1 --channel 0 --rate-hz 10 --mock` | Configuration says channel 0; exit 0 |
| `CLI-CHANNEL-1` | P | `$APP --samples 1 --channel 1 --mock` | Range `0..0`; exit 1 |
| `CLI-DRDY-0` | M | `$APP --check --drdy-line 0 --mock` | DRDY message says line 0; exit 0 |
| `CLI-DRDY-1023` | M | `$APP --check --drdy-line 1023 --mock` | DRDY message says line 1023; exit 0 |
| `CLI-DRDY-1024` | P | `$APP --check --drdy-line 1024 --mock` | Range `0..1023`; exit 1 |
| `CLI-CHECK-SAMPLES` | P | `$APP --check --samples 1 --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-SAMPLES-CHECK` | P | `$APP --samples 1 --check --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-CHECK-LIVE` | P | `$APP --check --live --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-LIVE-CHECK` | P | `$APP --live --check --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-HELP-CHECK` | P | `$APP --help --check --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-CHECK-HELP` | P | `$APP --check --help --mock` | Usage and no mode diagnostic; characterized order-dependent exit 0 |
| `CLI-REPEATED-CHECK` | P | `$APP --check --check --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-SAMPLES-LIVE` | P | `$APP --samples 1 --live --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-LIVE-SAMPLES` | P | `$APP --live --samples 1 --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-SAMPLES-HELP` | P | `$APP --samples 1 --help --mock` | Usage and no mode diagnostic; characterized order-dependent exit 0 |
| `CLI-HELP-SAMPLES` | P | `$APP --help --samples 1 --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-LIVE-HELP` | P | `$APP --live --help --mock` | Usage and no mode diagnostic; characterized order-dependent exit 0 |
| `CLI-HELP-LIVE` | P | `$APP --help --live --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-REPEATED-SAMPLES` | P | `$APP --samples 1 --samples 1 --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-REPEATED-LIVE` | P | `$APP --live --live --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-REPEATED-HELP` | P | `$APP --help --help` | Usage and no mode diagnostic; characterized exit 0 |
| `CLI-LIVE-CSV` | P | `$APP --live --csv unused.csv --mock` | Samples-only CSV diagnostic; exit 1 |
| `CLI-CSV-LIVE` | P | `$APP --csv unused.csv --live --mock` | Samples-only CSV diagnostic; exit 1 |
| `CLI-CHECK-CSV` | M | `$APP --check --csv unused.csv --mock` | Check runs and detects mock ADC; characterized accepted exit 0 |
| `CLI-CSV-CHECK` | M | `$APP --csv unused.csv --check --mock` | Check runs and detects mock ADC; characterized accepted exit 0 |
| `CLI-HELP-CSV` | P | `$APP --help --csv unused.csv` | Usage; characterized accepted exit 0 |
| `CLI-CSV-HELP` | P | `$APP --csv unused.csv --help` | Usage; characterized accepted exit 0 |
| `CLI-CSV-NO-MODE` | P | `$APP --csv unused.csv --mock` | Exactly-one-mode diagnostic; exit 1 |
| `CLI-CSV-SAMPLES` | M | `$APP --csv $CASE/reverse_order.csv --samples 1 --channel 0 --rate-hz 10 --mock` | Reverse-order CSV is created with header plus one row; exit 0 |
| `INT-STRACE-NO-DEVICE` | T | `strace ... $APP --check --mock` | App exit 0; trace exists and contains no `/dev/spidev*` or `/dev/gpiochip*` path |
| `INT-CSV-CLOSE-STRACE` | T | `strace -yy ... $APP --samples 1 --csv $CASE/closed.csv --mock` | CSV open descriptor is correlated to successful `close(fd)`, exact bytes and row; exit 0 |
| `INT-LIVE-SIGINT` | L | PTY/process group: `$APP --live --channel 0 --rate-hz 10 --mock`, then SIGINT | At least two records, stopped message, exit 0, no process group remains |
| `HAL-HAPPY-SAMPLE` | H | `FAKE_HAL_SCENARIO=happy_sample $FAKE --samples 1 --channel 0 --rate-hz 10` | Decoded raw 1234567, exact protocol/cleanup trace; exit 0 |
| `HAL-DELAYED-DRDY` | H | `FAKE_HAL_SCENARIO=delayed_drdy $FAKE --samples 1 --channel 0 --rate-hz 10` | Two busy reads then ready, one sample, exact trace; exit 0 |
| `HAL-READ-FAILURE` | H | `FAKE_HAL_SCENARIO=read_failure $FAKE --samples 1 --channel 0 --rate-hz 10` | SPI transfer failure diagnostic and exact cleanup trace; exit 1 |
| `HAL-PRESENCE-FAILURE` | H | `FAKE_HAL_SCENARIO=presence_failure $FAKE --samples 1 --channel 0 --rate-hz 10` | All-zero presence diagnostic, no configuration/sample transfer, exact cleanup trace; exit 1 |

## Cross-case limitations

- No scenario verifies physical SPI/GPIO, target execution, wiring, electrical
  behavior, ADS1263 accuracy, libgpiod, or real-time performance.
- The `samples=1000` maximum is parser-checked through the observed production
  help short-circuit; it deliberately does not run 1000 paced samples.
- Normal CSV close is verified by syscall descriptor correlation. Injected CSV
  write, flush, or close failures remain outside the existing boundary.

## Tool preconditions

- Linux and Python 3.10 or newer are mandatory for the runner, PTY, process
  groups, and native executable format.
- The manifest-recorded GCC executable/version is checked against the build
  provenance; it is mandatory to reproduce the build.
- `strace` is mandatory for `INT-STRACE-NO-DEVICE` and
  `INT-CSV-CLOSE-STRACE`. If unavailable, those cases and the harness summary
  report `BLOCKED`, not `FAIL` or a false pass.
- CLI help/order and CSV/check behavior are characterization results, not claims
  that the observed grammar satisfies an independently approved contract.
