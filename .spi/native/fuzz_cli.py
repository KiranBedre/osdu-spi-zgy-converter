import argparse
import random
import string
import subprocess
from collections.abc import Iterator
from pathlib import Path


def invalid_arguments(count: int) -> Iterator[list[str]]:
    rng = random.Random(0)
    fixed = [
        ["--osdu"],
        ["--verbosity", "not-a-number"],
        ["--brick-size", "-1"],
        ["--input", "/tmp/does-not-exist.sgy"],
    ]
    yield from fixed[:count]
    for _ in range(max(0, count - len(fixed))):
        token = "".join(rng.choices(string.ascii_letters + string.digits, k=24))
        yield [f"--spi-fuzz-{token}", token]


def run(binary: Path, count: int) -> None:
    executed = 0
    for arguments in invalid_arguments(count):
        try:
            result = subprocess.run(
                [binary, *arguments],
                stdout=subprocess.DEVNULL,
                stderr=subprocess.DEVNULL,
                check=False,
                timeout=1,
            )
        except subprocess.TimeoutExpired as error:
            raise RuntimeError(f"Converter hung for arguments {arguments!r}") from error
        if result.returncode < 0:
            raise RuntimeError(
                f"Converter exited from signal {-result.returncode} for {arguments!r}"
            )
        executed += 1
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
