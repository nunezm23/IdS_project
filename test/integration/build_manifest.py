#!/usr/bin/env python3
"""Create the integration build provenance manifest."""

from __future__ import annotations

import argparse
import datetime as dt
import hashlib
import json
from pathlib import Path


PRODUCTION_SOURCES = (
    "main/main.c",
    "components/acquisition/src/acquisition.c",
    "components/acquisition/include/acquisition.h",
    "components/ads1263/src/ads1263.c",
    "components/ads1263/include/ads1263.h",
    "components/ads1263/include/ads1263_defs.h",
    "components/logger/src/app_log.c",
    "components/logger/include/app_log.h",
    "components/ads1263/arch/linux/src/ads1263_spi_linux.c",
    "components/ads1263/arch/linux/src/ads1263_gpio_linux.c",
    "components/ads1263/arch/linux/include/ads1263_spi_linux.h",
    "components/ads1263/arch/linux/include/ads1263_gpio_linux.h",
)

TEST_SUPPORT_SOURCES = (
    "test/integration/build.sh",
    "test/integration/build_manifest.py",
    "test/integration/run.py",
    "test/integration/scenarios.md",
    "test/integration/README.md",
    "test/integration/fake_hal/fake_hal.c",
)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def read_argv(path: Path) -> list[str]:
    data = path.read_bytes()
    if not data.endswith(b"\0"):
        raise ValueError(f"invalid argv record: {path}")
    return [part.decode("utf-8") for part in data[:-1].split(b"\0")]


def source_records(repo_root: Path, paths: tuple[str, ...], classification: str) -> list[dict[str, str]]:
    records = []
    for relative in paths:
        source = repo_root / relative
        records.append(
            {
                "classification": classification,
                "path": relative,
                "absolute_path": str(source),
                "sha256": sha256(source),
            }
        )
    return records


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo-root", required=True, type=Path)
    parser.add_argument("--output-dir", required=True, type=Path)
    parser.add_argument("--compiler", required=True, type=Path)
    parser.add_argument("--compiler-version-file", required=True, type=Path)
    parser.add_argument("--real-argv-file", required=True, type=Path)
    parser.add_argument("--fake-argv-file", required=True, type=Path)
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    repo_root = args.repo_root.resolve(strict=True)
    output_dir = args.output_dir.resolve(strict=True)
    real_binary = output_dir / "ads1263_app"
    fake_binary = output_dir / "ads1263_app_fake_hal"
    manifest = {
        "schema_version": 1,
        "created_utc": dt.datetime.now(dt.timezone.utc).isoformat(),
        "repo_root": str(repo_root),
        "defines": ["_POSIX_C_SOURCE=200809L", "ADS1263_NO_GPIOD=1"],
        "compiler": {
            "path": str(args.compiler.resolve(strict=True)),
            "version": args.compiler_version_file.read_text(encoding="utf-8"),
            "version_argv": [str(args.compiler.resolve(strict=True)), "--version"],
        },
        "builds": {
            "native_linux_adapters": {
                "argv": read_argv(args.real_argv_file),
                "binary": {
                    "path": str(real_binary),
                    "sha256": sha256(real_binary),
                },
            },
            "scripted_fake_hal": {
                "argv": read_argv(args.fake_argv_file),
                "binary": {
                    "path": str(fake_binary),
                    "sha256": sha256(fake_binary),
                },
                "substitution": "test/integration/fake_hal/fake_hal.c replaces both Linux adapter objects",
            },
        },
        "sources": [
            *source_records(repo_root, PRODUCTION_SOURCES, "production"),
            *source_records(repo_root, TEST_SUPPORT_SOURCES, "test_support"),
        ],
    }
    (output_dir / "build_manifest.json").write_text(
        json.dumps(manifest, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
