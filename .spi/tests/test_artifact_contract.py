from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]


def test_native_image_is_pinned_tested_and_non_root() -> None:
    dockerfile = (REPO_ROOT / ".spi" / "image" / "Dockerfile").read_text(encoding="utf-8")

    assert "ubuntu:22.04@sha256:" in dockerfile
    assert "OPENZGY_SHA256=" in dockerfile
    assert "AZURE_SDK_SHA256=" in dockerfile
    assert "-DBUILD_AZURE_AUTH_PLUGIN=ON" in dockerfile
    assert "AS spi-test" in dockerfile
    assert "fuzz_cli.py --binary" in dockerfile
    assert "USER 10001:10001" in dockerfile
    assert "dev.azure.com" not in dockerfile


def test_companion_is_a_dag_only_layer() -> None:
    dockerfile = (REPO_ROOT / ".spi" / "dag" / "Dockerfile").read_text(encoding="utf-8")

    assert "python:3.12-slim-bookworm@sha256:" in dockerfile
    assert "FROM scratch" in dockerfile
    assert "COPY --from=render /out/ /dags/" in dockerfile
    assert "apache/airflow" not in dockerfile


def test_acceptance_sources_reach_the_script_image() -> None:
    ignored = (REPO_ROOT / ".dockerignore").read_text(encoding="utf-8").splitlines()

    assert ".spi/acceptance" not in ignored
    assert (REPO_ROOT / "build" / "script-acceptance.Dockerfile").is_file()
    assert (REPO_ROOT / "build" / "script-acceptance-entrypoint.js").is_file()
