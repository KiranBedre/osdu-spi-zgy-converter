import importlib.util
import subprocess
from pathlib import Path

import pytest

MODULE_PATH = Path(__file__).parents[1] / "native" / "fuzz_cli.py"
SPEC = importlib.util.spec_from_file_location("fuzz_cli", MODULE_PATH)
assert SPEC is not None and SPEC.loader is not None
fuzz_cli = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(fuzz_cli)


def test_invalid_arguments_are_deterministic_and_complete() -> None:
    first = list(fuzz_cli.invalid_arguments(64))
    second = list(fuzz_cli.invalid_arguments(64))

    assert first == second
    assert len(first) == 64
    assert all(len(case) == 4 for case in first)
    assert all(
        argument.startswith("/spi-fuzz-missing/") for case in first for argument in case
    )


def test_run_executes_every_case(monkeypatch: pytest.MonkeyPatch) -> None:
    calls: list[tuple[list[object], int]] = []

    def fake_run(command: list[object], **kwargs: object) -> subprocess.CompletedProcess:
        calls.append((command, int(kwargs["timeout"])))
        return subprocess.CompletedProcess(command, 1)

    monkeypatch.setattr(fuzz_cli.subprocess, "run", fake_run)

    fuzz_cli.run(Path("/converter"), 64)

    assert len(calls) == 64
    assert all(timeout == fuzz_cli.CASE_TIMEOUT_SECONDS for _, timeout in calls)


@pytest.mark.parametrize(
    ("returncode", "message"),
    [(-11, "signal 11"), (0, "accepted malformed arguments")],
)
def test_run_rejects_unsafe_outcomes(
    monkeypatch: pytest.MonkeyPatch, returncode: int, message: str
) -> None:
    def fake_run(command: list[object], **_: object) -> subprocess.CompletedProcess:
        return subprocess.CompletedProcess(command, returncode)

    monkeypatch.setattr(fuzz_cli.subprocess, "run", fake_run)

    with pytest.raises(RuntimeError, match=message):
        fuzz_cli.run(Path("/converter"), 1)


def test_run_rejects_timeout(monkeypatch: pytest.MonkeyPatch) -> None:
    def fake_run(command: list[object], **_: object) -> subprocess.CompletedProcess:
        raise subprocess.TimeoutExpired(command, fuzz_cli.CASE_TIMEOUT_SECONDS)

    monkeypatch.setattr(fuzz_cli.subprocess, "run", fake_run)

    with pytest.raises(RuntimeError, match="hung"):
        fuzz_cli.run(Path("/converter"), 1)
