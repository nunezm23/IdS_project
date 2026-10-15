#!/usr/bin/env python3
"""Process-level host integration runner using only the Python standard library."""

from __future__ import annotations

import argparse
import datetime as dt
import errno
import hashlib
import json
import os
import platform
import pty
import re
import select
import shutil
import signal
import subprocess
import sys
import time
from dataclasses import dataclass, field
from pathlib import Path
from typing import Any

sys.dont_write_bytecode = True
from build_manifest import PRODUCTION_SOURCES, TEST_SUPPORT_SOURCES


ROOT = Path(__file__).resolve().parents[2]
SCENARIO_FILE = Path(__file__).with_name("scenarios.md")
MANIFEST_NAME = "build_manifest.json"
REQUIRED_DEFINES = {"_POSIX_C_SOURCE=200809L", "ADS1263_NO_GPIOD=1"}
MOCK_SAMPLE_LINES = [
    "sample=001 raw=00001234567 voltage=0.001437 drdy=1 status=OK",
    "sample=002 raw=00002469134 voltage=0.002874 drdy=1 status=OK",
    "sample=003 raw=00003703701 voltage=0.004312 drdy=1 status=OK",
]
MOCK_CSV_ROWS = [
    "1,1234567,0.001437225,OK",
    "2,2469134,0.002874450,OK",
    "3,3703701,0.004311675,OK",
]
CSV_HEADER = "sample,raw,voltage,status"


@dataclass
class ProcessResult:
    command: list[str]
    stdout: str
    stderr: str
    exit_code: int
    timed_out: bool
    duration_seconds: float


@dataclass
class CaseResult:
    case_id: str
    command: list[str]
    stdout: str
    stderr: str
    exit_code: int
    timed_out: bool
    duration_seconds: float
    assertions: list[dict[str, Any]] = field(default_factory=list)
    blocked_reason: str | None = None

    def check(self, name: str, condition: bool, expected: Any, actual: Any) -> None:
        self.assertions.append(
            {"name": name, "passed": bool(condition), "expected": expected, "actual": actual}
        )

    @property
    def passed(self) -> bool:
        return self.state == "PASS"

    @property
    def state(self) -> str:
        if self.blocked_reason is not None:
            return "BLOCKED"
        return "PASS" if all(assertion["passed"] for assertion in self.assertions) else "FAIL"


class Evidence:
    def __init__(self, root: Path) -> None:
        self.root = root
        self.cases = root / "cases"
        self.results: list[CaseResult] = []

    def case_dir(self, case_id: str) -> Path:
        path = self.cases / case_id
        path.mkdir(parents=True, exist_ok=False)
        return path

    def record(self, result: CaseResult, case_dir: Path, environment: dict[str, str] | None = None) -> None:
        (case_dir / "stdout.txt").write_text(result.stdout, encoding="utf-8")
        (case_dir / "stderr.txt").write_text(result.stderr, encoding="utf-8")
        (case_dir / "exit_code.txt").write_text(f"{result.exit_code}\n", encoding="ascii")
        (case_dir / "assertions.json").write_text(
            json.dumps(result.assertions, indent=2, sort_keys=True) + "\n", encoding="utf-8"
        )
        command_record: dict[str, Any] = {
            "argv": result.command,
            "cwd": str(ROOT),
            "timeout_occurred": result.timed_out,
        }
        if environment:
            command_record["environment"] = environment
        (case_dir / "command.json").write_text(
            json.dumps(command_record, indent=2, sort_keys=True) + "\n", encoding="utf-8"
        )
        (case_dir / "result.json").write_text(
            json.dumps(
                {
                    "case_id": result.case_id,
                    "state": result.state,
                    "blocked_reason": result.blocked_reason,
                    "exit_code": result.exit_code,
                    "duration_seconds": round(result.duration_seconds, 6),
                    "assertion_count": len(result.assertions),
                    "assertions_passed": sum(a["passed"] for a in result.assertions),
                },
                indent=2,
                sort_keys=True,
            )
            + "\n",
            encoding="utf-8",
        )
        self.results.append(result)
        print(
            f"{result.case_id}: {result.state} "
            f"(exit={result.exit_code}, assertions={sum(a['passed'] for a in result.assertions)}/"
            f"{len(result.assertions)})"
        )


def signal_process_group(process: subprocess.Popen[Any], signum: signal.Signals) -> None:
    if process.poll() is not None:
        return
    try:
        os.killpg(process.pid, signum)
    except ProcessLookupError:
        pass


def terminate_process_group(process: subprocess.Popen[Any], grace: float = 0.25) -> None:
    """Terminate, escalate, and reap a child session while tolerating exit races."""
    try:
        if process.poll() is None:
            signal_process_group(process, signal.SIGTERM)
            try:
                process.wait(timeout=grace)
            except subprocess.TimeoutExpired:
                signal_process_group(process, signal.SIGKILL)
    finally:
        if process.poll() is None:
            try:
                process.wait(timeout=grace)
            except subprocess.TimeoutExpired:
                signal_process_group(process, signal.SIGKILL)
                process.wait()
        else:
            process.wait()


def run_process(command: list[str], timeout: float, environment: dict[str, str] | None = None) -> ProcessResult:
    env = os.environ.copy()
    if environment:
        env.update(environment)
    started = time.monotonic()
    process: subprocess.Popen[str] | None = None
    timed_out = False
    stdout = ""
    stderr = ""
    try:
        process = subprocess.Popen(
            command,
            cwd=ROOT,
            env=env,
            stdin=subprocess.DEVNULL,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            start_new_session=True,
            text=True,
        )
        try:
            stdout, stderr = process.communicate(timeout=timeout)
        except subprocess.TimeoutExpired:
            timed_out = True
            terminate_process_group(process)
            stdout, stderr = process.communicate()
    finally:
        if process is not None:
            terminate_process_group(process)
    if process is None:
        raise RuntimeError(f"failed to start process: {command}")
    return ProcessResult(
        command=command,
        stdout=stdout,
        stderr=stderr,
        exit_code=process.returncode,
        timed_out=timed_out,
        duration_seconds=time.monotonic() - started,
    )


def begin_case(evidence: Evidence, case_id: str, process: ProcessResult) -> tuple[CaseResult, Path]:
    result = CaseResult(
        case_id=case_id,
        command=process.command,
        stdout=process.stdout,
        stderr=process.stderr,
        exit_code=process.exit_code,
        timed_out=process.timed_out,
        duration_seconds=process.duration_seconds,
    )
    case_dir = evidence.case_dir(case_id)
    result.check("bounded execution", not process.timed_out, False, process.timed_out)
    return result, case_dir


def check_exit(result: CaseResult, expected: int) -> None:
    result.check("exit status", result.exit_code == expected, expected, result.exit_code)


def check_contains(result: CaseResult, stream: str, fragment: str) -> None:
    value = result.stdout if stream == "stdout" else result.stderr
    result.check(f"{stream} contains {fragment!r}", fragment in value, fragment, value)


def check_absent(result: CaseResult, stream: str, fragment: str) -> None:
    value = result.stdout if stream == "stdout" else result.stderr
    result.check(f"{stream} excludes {fragment!r}", fragment not in value, f"not {fragment}", value)


def sample_lines(stdout: str) -> list[str]:
    return [line for line in stdout.splitlines() if line.startswith("sample=")]


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def expected_csv(sample_count: int) -> bytes:
    return ("\n".join([CSV_HEADER, *MOCK_CSV_ROWS[:sample_count]]) + "\n").encode("ascii")


def run_mock_flows(binary: Path, evidence: Evidence) -> None:
    process = run_process([str(binary), "--check", "--mock"], 3.0)
    result, case_dir = begin_case(evidence, "INT-MOCK-CHECK", process)
    check_exit(result, 0)
    for fragment in ("ADS1263 register check", "ID        = 0x23", "ADS1263 detected", "DRDY:"):
        check_contains(result, "stdout", fragment)
    evidence.record(result, case_dir)

    process = run_process(
        [str(binary), "--samples", "3", "--channel", "0", "--rate-hz", "10", "--mock"], 3.0
    )
    result, case_dir = begin_case(evidence, "INT-MOCK-SAMPLES", process)
    lines = sample_lines(result.stdout)
    check_exit(result, 0)
    result.check("exact complete sample rows", lines == MOCK_SAMPLE_LINES, MOCK_SAMPLE_LINES, lines)
    evidence.record(result, case_dir)

    case_dir = evidence.case_dir("INT-CSV-CREATE")
    csv_path = case_dir / "created.csv"
    process = run_process(
        [str(binary), "--samples", "2", "--channel", "0", "--rate-hz", "10", "--csv", str(csv_path), "--mock"],
        3.0,
    )
    result = CaseResult("INT-CSV-CREATE", process.command, process.stdout, process.stderr, process.exit_code, process.timed_out, process.duration_seconds)
    result.check("bounded execution", not process.timed_out, False, process.timed_out)
    check_exit(result, 0)
    csv_bytes = csv_path.read_bytes() if csv_path.exists() else b""
    result.check("CSV created", csv_path.is_file(), True, csv_path.exists())
    result.check("exact CSV bytes", csv_bytes == expected_csv(2), expected_csv(2).decode("ascii"), csv_bytes.decode("ascii", errors="replace"))
    result.check("exact complete console sample rows", sample_lines(result.stdout) == MOCK_SAMPLE_LINES[:2], MOCK_SAMPLE_LINES[:2], sample_lines(result.stdout))
    evidence.record(result, case_dir)

    case_dir = evidence.case_dir("INT-CSV-TRUNCATE")
    csv_path = case_dir / "truncated.csv"
    csv_path.write_text("stale,data\nold,row\n", encoding="utf-8")
    process = run_process(
        [str(binary), "--samples", "1", "--channel", "0", "--rate-hz", "10", "--csv", str(csv_path), "--mock"],
        3.0,
    )
    result = CaseResult("INT-CSV-TRUNCATE", process.command, process.stdout, process.stderr, process.exit_code, process.timed_out, process.duration_seconds)
    result.check("bounded execution", not process.timed_out, False, process.timed_out)
    check_exit(result, 0)
    csv_bytes = csv_path.read_bytes() if csv_path.exists() else b""
    result.check("exact truncated CSV bytes", csv_bytes == expected_csv(1), expected_csv(1).decode("ascii"), csv_bytes.decode("ascii", errors="replace"))
    result.check("exact complete console sample row", sample_lines(result.stdout) == MOCK_SAMPLE_LINES[:1], MOCK_SAMPLE_LINES[:1], sample_lines(result.stdout))
    evidence.record(result, case_dir)

    case_dir = evidence.case_dir("INT-CSV-FAIL")
    blocker = case_dir / "not_a_directory"
    blocker.write_text("block", encoding="ascii")
    csv_path = blocker / "output.csv"
    process = run_process(
        [str(binary), "--samples", "2", "--channel", "0", "--rate-hz", "10", "--csv", str(csv_path), "--mock"],
        3.0,
    )
    result = CaseResult("INT-CSV-FAIL", process.command, process.stdout, process.stderr, process.exit_code, process.timed_out, process.duration_seconds)
    result.check("bounded execution", not process.timed_out, False, process.timed_out)
    check_exit(result, 0)
    check_contains(result, "stdout", "could not open CSV path")
    check_contains(result, "stdout", "continuing with console output only")
    result.check("exact complete console sample rows", sample_lines(result.stdout) == MOCK_SAMPLE_LINES[:2], MOCK_SAMPLE_LINES[:2], sample_lines(result.stdout))
    result.check("CSV absent", not csv_path.exists(), False, csv_path.exists())
    evidence.record(result, case_dir)

    case_dir = evidence.case_dir("CLI-CSV-SAMPLES")
    csv_path = case_dir / "reverse_order.csv"
    process = run_process(
        [str(binary), "--csv", str(csv_path), "--samples", "1", "--channel", "0", "--rate-hz", "10", "--mock"],
        3.0,
    )
    result = CaseResult("CLI-CSV-SAMPLES", process.command, process.stdout, process.stderr, process.exit_code, process.timed_out, process.duration_seconds)
    result.check("bounded execution", not process.timed_out, False, process.timed_out)
    check_exit(result, 0)
    result.check("reverse-order CSV created", csv_path.is_file(), True, csv_path.exists())
    csv_bytes = csv_path.read_bytes() if csv_path.exists() else b""
    result.check("exact reverse-order CSV bytes", csv_bytes == expected_csv(1), expected_csv(1).decode("ascii"), csv_bytes.decode("ascii", errors="replace"))
    result.check("exact complete console sample row", sample_lines(result.stdout) == MOCK_SAMPLE_LINES[:1], MOCK_SAMPLE_LINES[:1], sample_lines(result.stdout))
    evidence.record(result, case_dir)


def run_cli_cases(binary: Path, evidence: Evidence) -> None:
    cases: list[tuple[str, list[str], int, str, str, str | None]] = [
        ("CLI-NO-MODE", [], 1, "stdout", "Usage:", None),
        ("CLI-HELP", ["--help"], 0, "stdout", "Usage:", None),
        ("CLI-UNKNOWN", ["--check", "--mock", "--unknown"], 1, "stderr", "unknown option: --unknown", None),
        ("CLI-MISSING-SAMPLES", ["--samples"], 1, "stderr", "--samples requires a value", None),
        ("CLI-MISSING-CHANNEL", ["--check", "--mock", "--channel"], 1, "stderr", "--channel requires a value", None),
        ("CLI-MISSING-RATE", ["--check", "--mock", "--rate-hz"], 1, "stderr", "--rate-hz requires a value", None),
        ("CLI-MISSING-DRDY", ["--check", "--mock", "--drdy-line"], 1, "stderr", "--drdy-line requires a value", None),
        ("CLI-MISSING-CSV", ["--check", "--mock", "--csv"], 1, "stderr", "--csv requires a value", None),
        ("CLI-MISSING-SPI", ["--check", "--mock", "--spi-dev"], 1, "stderr", "--spi-dev requires a value", None),
        ("CLI-MISSING-GPIO", ["--check", "--mock", "--gpio-chip"], 1, "stderr", "--gpio-chip requires a value", None),
        ("CLI-MALFORMED-SAMPLES", ["--samples", "abc", "--mock"], 1, "stderr", "invalid samples value", None),
        ("CLI-MALFORMED-RATE", ["--samples", "1", "--rate-hz", "1x", "--mock"], 1, "stderr", "invalid rate-hz value", None),
        ("CLI-MALFORMED-CHANNEL", ["--samples", "1", "--channel", "-1", "--mock"], 1, "stderr", "invalid channel value", None),
        ("CLI-MALFORMED-DRDY", ["--check", "--drdy-line", "3.5", "--mock"], 1, "stderr", "invalid drdy-line value", None),
        ("CLI-EMPTY-SAMPLES", ["--samples", "", "--mock"], 1, "stderr", "samples requires a value", None),
        ("CLI-EMPTY-RATE", ["--samples", "1", "--rate-hz", "", "--mock"], 1, "stderr", "rate-hz requires a value", None),
        ("CLI-EMPTY-CHANNEL", ["--samples", "1", "--channel", "", "--mock"], 1, "stderr", "channel requires a value", None),
        ("CLI-EMPTY-DRDY", ["--check", "--drdy-line", "", "--mock"], 1, "stderr", "drdy-line requires a value", None),
        ("CLI-OVERFLOW-SAMPLES", ["--samples", "184467440737095516160", "--mock"], 1, "stderr", "invalid samples value", None),
        ("CLI-OVERFLOW-RATE", ["--samples", "1", "--rate-hz", "184467440737095516160", "--mock"], 1, "stderr", "invalid rate-hz value", None),
        ("CLI-OVERFLOW-CHANNEL", ["--samples", "1", "--channel", "184467440737095516160", "--mock"], 1, "stderr", "invalid channel value", None),
        ("CLI-OVERFLOW-DRDY", ["--check", "--drdy-line", "184467440737095516160", "--mock"], 1, "stderr", "invalid drdy-line value", None),
        ("CLI-SAMPLES-0", ["--samples", "0", "--mock"], 1, "stderr", "Valid range: 1..1000", None),
        ("CLI-SAMPLES-1", ["--samples", "1", "--channel", "0", "--rate-hz", "10", "--mock"], 0, "stdout", "sample=001", None),
        ("CLI-SAMPLES-1000", ["--samples", "1000", "--mock", "--help"], 0, "stdout", "Usage:", "invalid samples value"),
        ("CLI-SAMPLES-1001", ["--samples", "1001", "--mock"], 1, "stderr", "Valid range: 1..1000", None),
        ("CLI-RATE-0", ["--samples", "1", "--rate-hz", "0", "--mock"], 1, "stderr", "Valid range: 1..10", None),
        ("CLI-RATE-1", ["--samples", "1", "--rate-hz", "1", "--mock"], 0, "stdout", "rate_hz=1", None),
        ("CLI-RATE-10", ["--samples", "1", "--rate-hz", "10", "--mock"], 0, "stdout", "rate_hz=10", None),
        ("CLI-RATE-11", ["--samples", "1", "--rate-hz", "11", "--mock"], 1, "stderr", "Valid range: 1..10", None),
        ("CLI-CHANNEL-0", ["--samples", "1", "--channel", "0", "--rate-hz", "10", "--mock"], 0, "stdout", "channel=0", None),
        ("CLI-CHANNEL-1", ["--samples", "1", "--channel", "1", "--mock"], 1, "stderr", "Valid range: 0..0", None),
        ("CLI-DRDY-0", ["--check", "--drdy-line", "0", "--mock"], 0, "stdout", "line=0", None),
        ("CLI-DRDY-1023", ["--check", "--drdy-line", "1023", "--mock"], 0, "stdout", "line=1023", None),
        ("CLI-DRDY-1024", ["--check", "--drdy-line", "1024", "--mock"], 1, "stderr", "Valid range: 0..1023", None),
        ("CLI-CHECK-SAMPLES", ["--check", "--samples", "1", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-SAMPLES-CHECK", ["--samples", "1", "--check", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-CHECK-LIVE", ["--check", "--live", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-LIVE-CHECK", ["--live", "--check", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-HELP-CHECK", ["--help", "--check", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-CHECK-HELP", ["--check", "--help", "--mock"], 0, "stdout", "Usage:", "select exactly one command mode"),
        ("CLI-REPEATED-CHECK", ["--check", "--check", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-SAMPLES-LIVE", ["--samples", "1", "--live", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-LIVE-SAMPLES", ["--live", "--samples", "1", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-SAMPLES-HELP", ["--samples", "1", "--help", "--mock"], 0, "stdout", "Usage:", "select exactly one command mode"),
        ("CLI-HELP-SAMPLES", ["--help", "--samples", "1", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-LIVE-HELP", ["--live", "--help", "--mock"], 0, "stdout", "Usage:", "select exactly one command mode"),
        ("CLI-HELP-LIVE", ["--help", "--live", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-REPEATED-SAMPLES", ["--samples", "1", "--samples", "1", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-REPEATED-LIVE", ["--live", "--live", "--mock"], 1, "stderr", "select exactly one command mode", None),
        ("CLI-REPEATED-HELP", ["--help", "--help"], 0, "stdout", "Usage:", "select exactly one command mode"),
    ]

    csv_path = "unused.csv"
    cases.extend(
        [
            ("CLI-LIVE-CSV", ["--live", "--csv", csv_path, "--mock"], 1, "stderr", "--csv is supported with --samples mode only", None),
            ("CLI-CSV-LIVE", ["--csv", csv_path, "--live", "--mock"], 1, "stderr", "--csv is supported with --samples mode only", None),
            ("CLI-CHECK-CSV", ["--check", "--csv", csv_path, "--mock"], 0, "stdout", "ADS1263 detected", None),
            ("CLI-CSV-CHECK", ["--csv", csv_path, "--check", "--mock"], 0, "stdout", "ADS1263 detected", None),
            ("CLI-HELP-CSV", ["--help", "--csv", csv_path], 0, "stdout", "Usage:", None),
            ("CLI-CSV-HELP", ["--csv", csv_path, "--help"], 0, "stdout", "Usage:", None),
            ("CLI-CSV-NO-MODE", ["--csv", csv_path, "--mock"], 1, "stderr", "select exactly one command mode", None),
        ]
    )

    for case_id, arguments, expected_exit, stream, fragment, absent in cases:
        process = run_process([str(binary), *arguments], 3.0)
        result, case_dir = begin_case(evidence, case_id, process)
        check_exit(result, expected_exit)
        check_contains(result, stream, fragment)
        if absent is not None:
            check_absent(result, "stderr", absent)
        if sample_lines(result.stdout):
            result.check(
                "exact complete sample row",
                sample_lines(result.stdout) == MOCK_SAMPLE_LINES[:1],
                MOCK_SAMPLE_LINES[:1],
                sample_lines(result.stdout),
            )
        evidence.record(result, case_dir)


def record_blocked(evidence: Evidence, case_id: str, command: list[str], reason: str) -> None:
    case_dir = evidence.case_dir(case_id)
    result = CaseResult(case_id, command, "", f"BLOCKED: {reason}\n", 125, False, 0.0, blocked_reason=reason)
    evidence.record(result, case_dir)


def run_strace(binary: Path, evidence: Evidence, strace: str | None) -> None:
    case_id = "INT-STRACE-NO-DEVICE"
    command = [
        strace or "strace",
        "-f",
        "-qq",
        "-e",
        "trace=open,openat,openat2,ioctl",
        "-o",
        "TRACE_PATH",
        str(binary),
        "--check",
        "--mock",
    ]
    if strace is None:
        record_blocked(evidence, case_id, command, "mandatory tool precondition unavailable: strace")
        record_blocked(
            evidence,
            "INT-CSV-CLOSE-STRACE",
            ["strace", "...", str(binary), "--samples", "1", "--csv", "CSV_PATH", "--mock"],
            "mandatory tool precondition unavailable: strace",
        )
        return
    case_dir = evidence.case_dir(case_id)
    trace_path = case_dir / "strace.txt"
    command[command.index("TRACE_PATH")] = str(trace_path)
    process = run_process(command, 5.0)
    result = CaseResult(case_id, process.command, process.stdout, process.stderr, process.exit_code, process.timed_out, process.duration_seconds)
    result.check("bounded execution", not process.timed_out, False, process.timed_out)
    check_exit(result, 0)
    trace = trace_path.read_text(encoding="utf-8") if trace_path.exists() else ""
    result.check("trace captured", bool(trace), "nonempty strace output", len(trace))
    result.check("no spidev access", "/dev/spidev" not in trace, "no /dev/spidev path", trace)
    result.check("no gpiochip access", "/dev/gpiochip" not in trace, "no /dev/gpiochip path", trace)
    evidence.record(result, case_dir)

    case_id = "INT-CSV-CLOSE-STRACE"
    case_dir = evidence.case_dir(case_id)
    trace_path = case_dir / "strace.txt"
    csv_path = case_dir / "closed.csv"
    command = [
        strace,
        "-f",
        "-qq",
        "-yy",
        "-e",
        "trace=openat,close",
        "-o",
        str(trace_path),
        str(binary),
        "--samples",
        "1",
        "--channel",
        "0",
        "--rate-hz",
        "10",
        "--csv",
        str(csv_path),
        "--mock",
    ]
    process = run_process(command, 5.0)
    result = CaseResult(case_id, command, process.stdout, process.stderr, process.exit_code, process.timed_out, process.duration_seconds)
    result.check("bounded execution", not process.timed_out, False, process.timed_out)
    check_exit(result, 0)
    trace_lines = trace_path.read_text(encoding="utf-8").splitlines() if trace_path.exists() else []
    open_index = next((index for index, line in enumerate(trace_lines) if str(csv_path) in line and "openat(" in line), None)
    opened_fd: str | None = None
    if open_index is not None:
        match = re.search(r"=\s+(\d+)(?:<[^>]*>)?$", trace_lines[open_index])
        if match:
            opened_fd = match.group(1)
    close_line = None
    if opened_fd is not None and open_index is not None:
        close_pattern = re.compile(rf"close\({re.escape(opened_fd)}(?:<[^>]*>)?\)\s+=\s+0$")
        close_line = next((line for line in trace_lines[open_index + 1 :] if close_pattern.search(line)), None)
    result.check("CSV open fd identified", opened_fd is not None, "numeric CSV fd", trace_lines)
    result.check("CSV fd explicitly closed", close_line is not None, f"close({opened_fd}) = 0", close_line)
    csv_bytes = csv_path.read_bytes() if csv_path.exists() else b""
    result.check("exact closed CSV bytes", csv_bytes == expected_csv(1), expected_csv(1).decode("ascii"), csv_bytes.decode("ascii", errors="replace"))
    result.check("exact complete console sample row", sample_lines(result.stdout) == MOCK_SAMPLE_LINES[:1], MOCK_SAMPLE_LINES[:1], sample_lines(result.stdout))
    evidence.record(result, case_dir)


def run_live_pty(binary: Path, evidence: Evidence) -> None:
    case_id = "INT-LIVE-SIGINT"
    case_dir = evidence.case_dir(case_id)
    command = [str(binary), "--live", "--channel", "0", "--rate-hz", "10", "--mock"]
    started = time.monotonic()
    master_fd = -1
    slave_fd = -1
    process: subprocess.Popen[bytes] | None = None
    stdout_bytes = bytearray()
    stderr_bytes = bytearray()
    signal_sent = False
    timed_out = False
    startup_deadline = started + 3.0
    termination_deadline: float | None = None
    try:
        master_fd, slave_fd = pty.openpty()
        process = subprocess.Popen(
            command,
            cwd=ROOT,
            stdin=subprocess.DEVNULL,
            stdout=slave_fd,
            stderr=subprocess.PIPE,
            start_new_session=True,
        )
        os.close(slave_fd)
        slave_fd = -1
        stderr_fd = process.stderr.fileno() if process.stderr is not None else -1
        open_fds = {master_fd, stderr_fd}
        while open_fds or process.poll() is None:
            now = time.monotonic()
            if not signal_sent and now >= startup_deadline:
                timed_out = True
                break
            if signal_sent and termination_deadline is not None and now >= termination_deadline and process.poll() is None:
                timed_out = True
                break
            readable, _, _ = select.select([fd for fd in open_fds if fd >= 0], [], [], 0.05)
            for fd in readable:
                try:
                    chunk = os.read(fd, 4096)
                except OSError as error:
                    if error.errno == errno.EIO:
                        chunk = b""
                    else:
                        raise
                if not chunk:
                    open_fds.discard(fd)
                elif fd == master_fd:
                    stdout_bytes.extend(chunk)
                else:
                    stderr_bytes.extend(chunk)
            if not signal_sent and stdout_bytes.count(b"sample=") >= 2:
                signal_process_group(process, signal.SIGINT)
                signal_sent = True
                termination_deadline = time.monotonic() + 2.0
            if process.poll() is not None and not readable:
                for fd in list(open_fds):
                    try:
                        chunk = os.read(fd, 4096)
                    except OSError:
                        chunk = b""
                    if chunk:
                        (stdout_bytes if fd == master_fd else stderr_bytes).extend(chunk)
                    else:
                        open_fds.discard(fd)
    finally:
        if process is not None:
            terminate_process_group(process)
        for fd in (master_fd, slave_fd):
            if fd >= 0:
                try:
                    os.close(fd)
                except OSError:
                    pass
        if process is not None and process.stderr is not None:
            process.stderr.close()

    if process is None:
        raise RuntimeError("failed to start PTY process")

    group_gone = False
    try:
        os.killpg(process.pid, 0)
    except ProcessLookupError:
        group_gone = True
    stdout = stdout_bytes.decode("utf-8", errors="replace").replace("\r\n", "\n")
    stderr = stderr_bytes.decode("utf-8", errors="replace")
    result = CaseResult(case_id, command, stdout, stderr, process.returncode, timed_out, time.monotonic() - started)
    result.check("minimum records observed before SIGINT", signal_sent, True, signal_sent)
    result.check("exact complete live sample rows", sample_lines(stdout) == MOCK_SAMPLE_LINES[:2], MOCK_SAMPLE_LINES[:2], sample_lines(stdout))
    result.check("bounded clean termination", not timed_out, False, timed_out)
    check_exit(result, 0)
    check_contains(result, "stdout", "live acquisition stopped")
    result.check("process group cleaned up", group_gone, True, group_gone)
    evidence.record(result, case_dir)


def fake_expected(scenario_name: str) -> list[str]:
    lines = [
        "SPI_OPEN device=/dev/spidev0.0 speed=1000000 result=OK",
        "GPIO_OPEN chip=/dev/gpiochip6 line=8 result=OK",
        "SPI_TRANSFER len=7 tx=20:04:00:00:00:00:00 result=OK",
    ]
    if scenario_name == "presence_failure":
        return [*lines, "GPIO_CLOSE", "SPI_CLOSE"]
    lines.extend(
        [
            "SPI_TRANSFER len=3 tx=46:00:0A result=OK",
            "SPI_TRANSFER len=3 tx=26:00:00 result=OK",
            "SPI_TRANSFER len=1 tx=08 result=OK",
        ]
    )
    if scenario_name == "delayed_drdy":
        lines.extend(["GPIO_READ value=1 result=OK", "GPIO_READ value=1 result=OK"])
    lines.append("GPIO_READ value=0 result=OK")
    sample_result = "SPI_TRANSFER" if scenario_name == "read_failure" else "OK"
    lines.append(f"SPI_TRANSFER len=7 tx=12:00:00:00:00:00:00 result={sample_result}")
    lines.extend(["GPIO_CLOSE", "SPI_CLOSE"])
    return lines


def run_fake_hal(fake_binary: Path, evidence: Evidence) -> None:
    cases = [
        ("HAL-HAPPY-SAMPLE", "happy_sample", 0, "stdout", "raw=00001234567"),
        ("HAL-DELAYED-DRDY", "delayed_drdy", 0, "stdout", "sample=001"),
        ("HAL-READ-FAILURE", "read_failure", 1, "stderr", "sample read failed: SPI_TRANSFER"),
        ("HAL-PRESENCE-FAILURE", "presence_failure", 1, "stderr", "not detected: all registers read as 0x00"),
    ]
    for case_id, scenario_name, expected_exit, stream, fragment in cases:
        case_dir = evidence.case_dir(case_id)
        trace_path = case_dir / "hal_trace.txt"
        environment = {"FAKE_HAL_SCENARIO": scenario_name, "FAKE_HAL_TRACE": str(trace_path)}
        command = [str(fake_binary), "--samples", "1", "--channel", "0", "--rate-hz", "10"]
        process = run_process(command, 3.0, environment)
        result = CaseResult(case_id, command, process.stdout, process.stderr, process.exit_code, process.timed_out, process.duration_seconds)
        result.check("bounded execution", not process.timed_out, False, process.timed_out)
        check_exit(result, expected_exit)
        check_contains(result, stream, fragment)
        if scenario_name in {"happy_sample", "delayed_drdy"}:
            expected_sample = [MOCK_SAMPLE_LINES[0].replace("drdy=1", "drdy=0")]
            result.check("exact complete sample row", sample_lines(result.stdout) == expected_sample, expected_sample, sample_lines(result.stdout))
        actual_trace = trace_path.read_text(encoding="utf-8").splitlines() if trace_path.exists() else []
        expected_trace = fake_expected(scenario_name)
        result.check("strict HAL event trace", actual_trace == expected_trace, expected_trace, actual_trace)
        result.check(
            "HAL resources closed in order",
            actual_trace[-2:] == ["GPIO_CLOSE", "SPI_CLOSE"] if len(actual_trace) >= 2 else False,
            ["GPIO_CLOSE", "SPI_CLOSE"],
            actual_trace[-2:],
        )
        evidence.record(result, case_dir, environment)


def parse_scenario_declaration() -> tuple[set[str], dict[str, str], list[str]]:
    text = SCENARIO_FILE.read_text(encoding="utf-8")
    case_ids = set(re.findall(r"^\| `([A-Z0-9-]+)` \|", text, flags=re.MULTILINE))
    fields = {
        key.lower().replace("-", "_"): value.strip()
        for key, value in re.findall(r"^Approval-([A-Za-z-]+):\s*(.+)$", text, flags=re.MULTILINE)
    }
    errors = []
    required = {"basis", "reviewer", "disposition", "date", "scope"}
    missing = sorted(required - fields.keys())
    if missing:
        errors.append(f"missing approval fields: {', '.join(missing)}")
    if fields.get("disposition") != "APPROVED":
        errors.append("approval disposition must be APPROVED")
    if fields.get("scope") != "all declared scenario IDs":
        errors.append("approval scope must cover all declared scenario IDs")
    if "current user authorization" not in fields.get("basis", "").lower():
        errors.append("approval basis must identify the current user authorization")
    try:
        approval_date = dt.date.fromisoformat(fields.get("date", ""))
        if approval_date > dt.datetime.now(dt.timezone.utc).date():
            errors.append("approval date must not be in the future")
    except ValueError:
        errors.append("approval date must be ISO-8601 YYYY-MM-DD")
    if not fields.get("reviewer"):
        errors.append("approval reviewer must be recorded")
    if not case_ids:
        errors.append("no declared scenario IDs found")
    return case_ids, fields, errors


def validate_directory(path: Path, label: str, require_empty: bool) -> Path:
    if not path.exists() or not path.is_dir():
        raise ValueError(f"{label} must already exist as a directory: {path}")
    if path.is_symlink():
        raise ValueError(f"{label} must not be a symbolic link: {path}")
    lexical = Path(os.path.abspath(os.path.normpath(path)))
    physical = path.resolve(strict=True)
    if lexical != physical:
        raise ValueError(f"{label} must not contain symbolic-link path components: {path}")
    if require_empty and any(physical.iterdir()):
        raise ValueError(f"{label} must be empty to prevent clobbering: {physical}")
    return physical


def validate_manifest(manifest: dict[str, Any], binary_dir: Path) -> list[str]:
    errors: list[str] = []
    if manifest.get("schema_version") != 1:
        errors.append("unsupported manifest schema_version")
    if manifest.get("repo_root") != str(ROOT):
        errors.append("manifest repo_root does not match this repository")
    if set(manifest.get("defines", [])) != REQUIRED_DEFINES:
        errors.append("manifest does not contain exactly the two required defines")

    expected_sources = {
        **{path: "production" for path in PRODUCTION_SOURCES},
        **{path: "test_support" for path in TEST_SUPPORT_SOURCES},
    }
    records = manifest.get("sources")
    if not isinstance(records, list):
        errors.append("manifest sources must be a list")
        records = []
    actual_paths = [record.get("path") for record in records if isinstance(record, dict)]
    if set(actual_paths) != set(expected_sources) or len(actual_paths) != len(expected_sources):
        errors.append("manifest source inventory does not exactly match required production/test support inputs")
    for record in records:
        if not isinstance(record, dict):
            errors.append("manifest contains a malformed source record")
            continue
        relative = record.get("path")
        if relative not in expected_sources:
            continue
        source = ROOT / relative
        if record.get("classification") != expected_sources[relative]:
            errors.append(f"source classification mismatch: {relative}")
        if record.get("absolute_path") != str(source):
            errors.append(f"source absolute path mismatch: {relative}")
        if not source.is_file() or record.get("sha256") != sha256(source):
            errors.append(f"source hash mismatch: {relative}")

    builds = manifest.get("builds")
    if not isinstance(builds, dict):
        errors.append("manifest builds must be an object")
        builds = {}
    expected_builds = {
        "native_linux_adapters": binary_dir / "ads1263_app",
        "scripted_fake_hal": binary_dir / "ads1263_app_fake_hal",
    }
    compiler = manifest.get("compiler")
    compiler_path = compiler.get("path") if isinstance(compiler, dict) else None
    if not isinstance(compiler_path, str) or not isinstance(compiler, dict) or not compiler.get("version"):
        errors.append("manifest compiler path/version is missing")
    elif compiler.get("version_argv") != [compiler_path, "--version"]:
        errors.append("manifest compiler version argv mismatch")
    common_argv = [
        compiler_path,
        "-std=c11",
        "-O2",
        "-g",
        "-Wall",
        "-Wextra",
        "-pedantic",
        "-D_POSIX_C_SOURCE=200809L",
        "-DADS1263_NO_GPIOD=1",
        f"-I{ROOT / 'components/logger/include'}",
        f"-I{ROOT / 'components/ads1263/include'}",
        f"-I{ROOT / 'components/ads1263/arch/linux/include'}",
        f"-I{ROOT / 'components/acquisition/include'}",
    ]
    core_sources = [
        str(ROOT / "main/main.c"),
        str(ROOT / "components/acquisition/src/acquisition.c"),
        str(ROOT / "components/ads1263/src/ads1263.c"),
        str(ROOT / "components/logger/src/app_log.c"),
    ]
    expected_argv = {
        "native_linux_adapters": [
            *common_argv,
            *core_sources,
            str(ROOT / "components/ads1263/arch/linux/src/ads1263_spi_linux.c"),
            str(ROOT / "components/ads1263/arch/linux/src/ads1263_gpio_linux.c"),
            "-o",
            str(binary_dir / "ads1263_app"),
        ],
        "scripted_fake_hal": [
            *common_argv,
            *core_sources,
            str(ROOT / "test/integration/fake_hal/fake_hal.c"),
            "-o",
            str(binary_dir / "ads1263_app_fake_hal"),
        ],
    }
    for build_name, expected_binary in expected_builds.items():
        build = builds.get(build_name)
        if not isinstance(build, dict):
            errors.append(f"missing build record: {build_name}")
            continue
        argv = build.get("argv")
        binary_record = build.get("binary")
        if not isinstance(argv, list) or not all(isinstance(value, str) for value in argv):
            errors.append(f"invalid compiler argv: {build_name}")
            argv = []
        for define in REQUIRED_DEFINES:
            if f"-D{define}" not in argv:
                errors.append(f"compiler argv missing {define}: {build_name}")
        if compiler_path is not None and (not argv or argv[0] != compiler_path):
            errors.append(f"compiler argv path mismatch: {build_name}")
        if argv != expected_argv[build_name]:
            errors.append(f"compiler argv does not exactly match the approved build: {build_name}")
        if not isinstance(binary_record, dict):
            errors.append(f"missing binary record: {build_name}")
            continue
        if binary_record.get("path") != str(expected_binary):
            errors.append(f"binary path mismatch: {build_name}")
        if not expected_binary.is_file() or not os.access(expected_binary, os.X_OK):
            errors.append(f"binary is missing or not executable: {expected_binary}")
        elif binary_record.get("sha256") != sha256(expected_binary):
            errors.append(f"binary hash mismatch: {build_name}")
    return errors


def check_tool_preconditions(manifest: dict[str, Any]) -> tuple[dict[str, Any], str | None]:
    compiler = manifest["compiler"]
    compiler_path = Path(compiler["path"])
    compiler_status = "BLOCKED"
    compiler_actual = None
    if compiler_path.is_file() and os.access(compiler_path, os.X_OK):
        version_result = run_process([str(compiler_path), "--version"], 3.0)
        compiler_actual = version_result.stdout
        if not version_result.timed_out and version_result.exit_code == 0 and compiler_actual == compiler["version"]:
            compiler_status = "PASS"
    strace = shutil.which("strace")
    strace_version = None
    strace_status = "BLOCKED"
    if strace is not None:
        version_result = run_process([strace, "--version"], 3.0)
        strace_version = version_result.stdout.splitlines()[0] if version_result.stdout else version_result.stderr.splitlines()[0]
        if not version_result.timed_out and version_result.exit_code == 0:
            strace_status = "PASS"
    linux_status = "PASS" if sys.platform.startswith("linux") else "BLOCKED"
    python_status = "PASS" if sys.version_info >= (3, 10) else "BLOCKED"
    preconditions = {
        "gcc_build_provenance": {
            "mandatory_for": "reproducing the build",
            "status": compiler_status,
            "path": str(compiler_path),
            "expected_version": compiler["version"],
            "actual_version": compiler_actual,
        },
        "python": {
            "mandatory_for": "runner",
            "status": python_status,
            "minimum": "3.10",
            "actual": platform.python_version(),
        },
        "linux": {
            "mandatory_for": "native binaries, PTY, process groups, and strace",
            "status": linux_status,
            "actual": sys.platform,
        },
        "strace": {
            "mandatory_for": "INT-STRACE-NO-DEVICE and INT-CSV-CLOSE-STRACE",
            "status": strace_status,
            "path": strace,
            "version": strace_version,
        },
    }
    return preconditions, strace if strace_status == "PASS" else None


def git_output(*arguments: str) -> str:
    git = shutil.which("git")
    if git is None:
        return "UNAVAILABLE: git not found in PATH"
    result = run_process([git, *arguments], 3.0)
    output = result.stdout + result.stderr
    if result.timed_out:
        return "UNAVAILABLE: git command timed out"
    return output.rstrip("\n")


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--binary-dir", required=True, type=Path, help="Directory produced by build.sh")
    parser.add_argument("--evidence-dir", required=True, type=Path, help="New or empty caller-provided evidence directory")
    return parser.parse_args()


def main() -> int:
    args = parse_args()
    try:
        binary_dir = validate_directory(args.binary_dir, "binary directory", require_empty=False)
        evidence_dir = validate_directory(args.evidence_dir, "evidence directory", require_empty=True)
    except ValueError as error:
        raise SystemExit(str(error)) from error
    if binary_dir.is_relative_to(ROOT) or evidence_dir.is_relative_to(ROOT):
        raise SystemExit("binary and evidence directories must be outside the repository")

    manifest_path = binary_dir / MANIFEST_NAME
    if not manifest_path.is_file() or manifest_path.is_symlink():
        raise SystemExit(f"required regular build manifest is missing: {manifest_path}")
    try:
        manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError) as error:
        raise SystemExit(f"could not read build manifest: {error}") from error

    shutil.copyfile(manifest_path, evidence_dir / MANIFEST_NAME)
    shutil.copyfile(SCENARIO_FILE, evidence_dir / "scenarios.md")
    manifest_errors = validate_manifest(manifest, binary_dir)
    declared, approval, approval_errors = parse_scenario_declaration()
    validation = {
        "state": "PASS" if not manifest_errors and not approval_errors else "FAIL",
        "manifest_sha256": sha256(manifest_path),
        "manifest_errors": manifest_errors,
        "scenario_approval": approval,
        "scenario_approval_errors": approval_errors,
    }
    (evidence_dir / "provenance_validation.json").write_text(
        json.dumps(validation, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    if manifest_errors or approval_errors:
        summary = {
            "state": "FAIL",
            "harness_error": "build provenance or scenario approval validation failed before execution",
            "scenario_count": 0,
            "passed": 0,
            "failed": 0,
            "blocked": 0,
            "declared_scenario_count": len(declared),
            "provenance_validation": validation,
        }
        (evidence_dir / "summary.json").write_text(
            json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8"
        )
        print("SUMMARY: FAIL provenance/approval validation prevented execution")
        return 1

    binary = binary_dir / "ads1263_app"
    fake_binary = binary_dir / "ads1263_app_fake_hal"
    evidence = Evidence(evidence_dir)

    started = dt.datetime.now(dt.timezone.utc)
    preconditions, strace = check_tool_preconditions(manifest)
    (evidence_dir / "tool_preconditions.json").write_text(
        json.dumps(preconditions, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    metadata = {
        "classification": "software-only; physical SPI, GPIO, ADC, target, and real-time behavior not verified",
        "command": [sys.executable, *sys.argv],
        "cwd": str(ROOT),
        "git_head": git_output("rev-parse", "HEAD"),
        "git_status_short": git_output("status", "--short"),
        "host": platform.platform(),
        "machine": platform.machine(),
        "python": platform.python_version(),
        "started_utc": started.isoformat(),
        "binary_dir": str(binary_dir),
        "build_manifest": MANIFEST_NAME,
        "build_manifest_sha256": sha256(manifest_path),
        "scenario_approval": approval,
        "boundaries": {
            "built_in_mock": "real main, acquisition, ADS1263 core, logger, Linux adapters linked; ADS1263 mock branch simulated",
            "fake_hal": "real main, acquisition, ADS1263 core, logger; SPI/GPIO adapter symbols scripted",
        },
    }
    (evidence_dir / "metadata.json").write_text(
        json.dumps(metadata, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )

    run_mock_flows(binary, evidence)
    run_cli_cases(binary, evidence)
    run_strace(binary, evidence, strace)
    run_live_pty(binary, evidence)
    run_fake_hal(fake_binary, evidence)

    implemented = {result.case_id for result in evidence.results}
    declaration_matches = implemented == declared
    finished = dt.datetime.now(dt.timezone.utc)
    passed = sum(result.state == "PASS" for result in evidence.results)
    failed = sum(result.state == "FAIL" for result in evidence.results)
    blocked = sum(result.state == "BLOCKED" for result in evidence.results)
    blocked_preconditions = sorted(
        name for name, record in preconditions.items() if record.get("status") == "BLOCKED"
    )
    if failed != 0 or not declaration_matches:
        overall_state = "FAIL"
    elif blocked != 0 or blocked_preconditions:
        overall_state = "BLOCKED"
    else:
        overall_state = "PASS"
    summary = {
        "state": overall_state,
        "scenario_count": len(evidence.results),
        "passed": passed,
        "failed": failed,
        "blocked": blocked,
        "blocked_preconditions": blocked_preconditions,
        "declared_scenario_count": len(declared),
        "declaration_matches_execution": declaration_matches,
        "undeclared_executed": sorted(implemented - declared),
        "declared_not_executed": sorted(declared - implemented),
        "started_utc": started.isoformat(),
        "finished_utc": finished.isoformat(),
        "duration_seconds": round((finished - started).total_seconds(), 6),
        "cases": [
            {
                "case_id": result.case_id,
                "state": result.state,
                "blocked_reason": result.blocked_reason,
                "exit_code": result.exit_code,
                "assertions": len(result.assertions),
                "assertions_passed": sum(a["passed"] for a in result.assertions),
            }
            for result in evidence.results
        ],
        "limitations": [
            "No physical SPI, GPIO, ADS1263, target deployment, electrical behavior, ADC accuracy, or real-time timing is verified.",
            "Built-in mock bypasses SPI framing and GPIO polling; fake-HAL cases exercise framing and orchestration but not Linux ioctl/libgpiod behavior.",
            "The samples=1000 CLI maximum is parser-checked through the production help short-circuit to avoid a roughly 100-second paced acquisition.",
            "Normal CSV close is syscall-correlated, but injected write, flush, or close failures remain outside this process suite.",
        ],
    }
    (evidence_dir / "summary.json").write_text(
        json.dumps(summary, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(
        f"SUMMARY: {summary['state']} "
        f"scenarios={len(evidence.results)} passed={passed} failed={failed} blocked={blocked} "
        f"declaration_match={declaration_matches}"
    )
    print(f"Summary JSON: {evidence_dir / 'summary.json'}")
    return 0 if summary["state"] == "PASS" else 1


if __name__ == "__main__":
    raise SystemExit(main())
