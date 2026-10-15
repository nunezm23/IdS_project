# Host software integration

This directory builds and drives unchanged production translation units. Build
artifacts and execution evidence must be placed in existing, empty,
non-symlinked directories outside the repository.

```sh
build_dir=$(mktemp -d /tmp/opencode/ids-integration-build.XXXXXX)
evidence_dir=$(mktemp -d /tmp/opencode/ids-integration-evidence.XXXXXX)
test/integration/build.sh "$build_dir"
python3 test/integration/run.py --binary-dir "$build_dir" --evidence-dir "$evidence_dir"
```

`build.sh` creates two executables. `ads1263_app` links unchanged `main/main.c`,
`acquisition.c`, `ads1263.c`, `app_log.c`, and both unchanged Linux adapters
with `ADS1263_NO_GPIOD=1`. `ads1263_app_fake_hal` links the same four core
production files but substitutes the Linux adapter objects with
`fake_hal/fake_hal.c`. It also writes `build_manifest.json` with exact compiler
argv, `_POSIX_C_SOURCE=200809L`, `ADS1263_NO_GPIOD=1`, compiler version,
production/test-support source paths and SHA-256 hashes, and both binary hashes.

The runner uses only the Python standard library. Before any scenario it checks
the manifest schema, complete source inventory/hashes, exact defines, compiler
argv records, binary paths/hashes, and scenario approval. It preserves the
manifest and declaration in evidence, rejects nonempty or symlinked directories,
records command/stdout/stderr/exit/assertions for every case, and writes
`summary.json`. See `scenarios.md` for approved declarations and preconditions.

## Scripted HAL feasibility

The task 11.6 gate is **feasible and accepted** for this narrow scope:

- The production driver already calls six externally linked SPI/GPIO adapter
  functions, so substitution needs no production seam or source change.
- Real CLI, acquisition orchestration, register framing, DRDY polling, sample
  decoding, logging, failure propagation, and cleanup remain in the process.
- Four scripts add integration value beyond built-in mock mode: one sample,
  delayed DRDY, sample transfer failure, and presence failure.
- The fake accepts only exact expected frames and emits a strict ordered trace.
  Any extra, missing, or reordered event fails the runner.
- Setup is one C support file and one alternate link command. It has no target
  SDK, libgpiod, device node, root privilege, or hardware dependency.

This acceptance does not validate Linux `open`/`ioctl`/libgpiod behavior or any
physical, electrical, ADC-accuracy, target, or real-time property.
