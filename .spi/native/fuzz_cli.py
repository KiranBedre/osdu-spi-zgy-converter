import argparse
import random
import string
import subprocess
from collections.abc import Iterator
from concurrent.futures import ThreadPoolExecutor
from functools import partial
from pathlib import Path

CASE_TIMEOUT_SECONDS = 35
MAX_WORKERS = 8


def invalid_arguments(count: int) -> Iterator[list[str]]:
    rng = random.Random(0)
    for _ in range(count):
        token = "".join(rng.choices(string.ascii_letters + string.digits, k=24))
        yield [
            f"/spi-fuzz-missing/{token}.sgy",
            f"/spi-fuzz-missing/{token}.json",
            f"/spi-fuzz-missing/{token}-grid.json",
            f"/spi-fuzz-missing/{token}.zgy",
        ]


def run_case(binary: Path, arguments: list[str]) -> None:
    try:
        result = subprocess.run(
            [binary, *arguments],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
            check=False,
            timeout=CASE_TIMEOUT_SECONDS,
        )
    except subprocess.TimeoutExpired as error:
        raise RuntimeError(f"Converter hung for arguments {arguments!r}") from error
    if result.returncode < 0:
        raise RuntimeError(
            f"Converter exited from signal {-result.returncode} for {arguments!r}"
        )
    if result.returncode == 0:
        raise RuntimeError(f"Converter accepted malformed arguments {arguments!r}")


def run(binary: Path, count: int) -> None:
    cases = list(invalid_arguments(count))
    with ThreadPoolExecutor(max_workers=min(MAX_WORKERS, count)) as executor:
        list(executor.map(partial(run_case, binary), cases))
    executed = len(cases)
    if executed != count:
        raise RuntimeError(f"Expected {count} fuzz cases, executed {executed}")
    print(f"Executed {executed} bounded converter argument fuzz cases")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--binary", type=Path, required=True)
    parser.add_argument("--cases", type=int, default=64)
    args = parser.parse_args()
    if args.cases < 1:
        parser.error("--cases must be positive")
    run(args.binary, args.cases)
    return 0
if __name__ == "__main__":
    raise SystemExit(main())
    raise SystemExit(main())
