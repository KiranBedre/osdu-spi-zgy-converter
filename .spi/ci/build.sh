#!/usr/bin/env bash

set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
cd "$repo_root"

export PYTHONPATH="$repo_root/.spi/ci${PYTHONPATH:+:$PYTHONPATH}"

python -m spi_zgy.render_dag \
  --source airflow/workflow-svc-v2/segy_to_zgy_ingestion_dag.py \
  --output .spi/generated/segy_to_zgy_ingestion_dag.py

python -m ruff check --line-length 100 .spi/ci/spi_zgy .spi/native .spi/tests
node --test .spi/tests/*.test.mjs
python -m pytest .spi/tests \
  --junitxml "$SPI_TEST_REPORT" \
  --cov spi_zgy \
  --cov-report "xml:$SPI_COVERAGE_FILE" \
  --cov-fail-under 90
