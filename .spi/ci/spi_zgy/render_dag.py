import re
from argparse import ArgumentParser
from pathlib import Path

POD_OPTIONS = """{
    "annotations": {"sidecar.istio.io/inject": "false"},
    "labels": {"azure.workload.identity/use": "true"},
    "service_account_name": "workload-identity-sa",
    "node_selector": {"spi-pool": "osdu"},
    "tolerations": [
        k8s.V1Toleration(
            key="workload", operator="Equal", value="osdu", effect="NoSchedule"
        ),
        k8s.V1Toleration(
            key="kubernetes.azure.com/scalesetpriority",
            operator="Equal",
            value="spot",
            effect="NoSchedule",
        ),
    ],
    "security_context": k8s.V1PodSecurityContext(
        run_as_non_root=True,
        run_as_user=10001,
        run_as_group=10001,
        fs_group=10001,
        seccomp_profile=k8s.V1SeccompProfile(type="RuntimeDefault"),
    ),
    "container_security_context": k8s.V1SecurityContext(
        allow_privilege_escalation=False,
        capabilities=k8s.V1Capabilities(drop=["ALL"]),
    ),
    "container_resources": k8s.V1ResourceRequirements(
        requests={"cpu": "1", "memory": "2Gi"},
        limits={"cpu": "3", "memory": "6Gi"},
    ),
    "startup_timeout_seconds": 300,
}"""

EXTRA_ENV = """{
    "AUTHENTICATION_PLUGIN_FILE": "/usr/local/bin/segy/libsegysdk.auth_plugin.Azure.so"
}"""

REPLACEMENTS = {
    "{| DAG_NAME |}": "segy-to-zgy-conversion",
    "{| DOCKER_IMAGE |}": "{{ var.value.zgy_converter_image }}",
    "{| NAMESPACE |}": "platform",
    "{|  K8S_POD_OPERATOR_KWARGS |}": POD_OPTIONS,
    "{| STORAGE_SVC_URL |}": "http://storage.osdu.svc.cluster.local/api/storage/v2",
    "{| SD_SVC_URL |}": "http://seismic.osdu.svc.cluster.local/seistore-svc/api/v3",
    "{| EXTRA_ENV_VARS|default('{}') |}": EXTRA_ENV,
    '"SD_SVC_TOKEN": sd_svc_token,': '"SD_SVC_TOKEN": "",',
    '"STORAGE_SVC_TOKEN": authorization,': '"STORAGE_SVC_TOKEN": "",',
    '"STORAGE_SVC_API_KEY": storage_svc_api_key,': '"STORAGE_SVC_API_KEY": "",',
    '"SD_SVC_API_KEY": sd_svc_api_key,': '"SD_SVC_API_KEY": "",',
    "from airflow import DAG": (
        "from airflow import DAG\nfrom kubernetes.client import models as k8s"
    ),
}
UNRESOLVED_TOKEN = re.compile(r"\{\|.*?\|\}")


def render_dag(source: str) -> str:
    rendered = source
    for token, value in REPLACEMENTS.items():
        occurrences = rendered.count(token)
        if occurrences != 1:
            raise ValueError(f"Expected one occurrence of {token!r}, found {occurrences}")
        rendered = rendered.replace(token, value)

    unresolved = sorted(set(UNRESOLVED_TOKEN.findall(rendered)))
    if unresolved:
        raise ValueError(f"Unresolved DAG template tokens: {', '.join(unresolved)}")
    return rendered


def render_file(source: Path, output: Path) -> None:
    rendered = render_dag(source.read_text(encoding="utf-8"))
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_text(rendered, encoding="utf-8")


def main() -> int:
    parser = ArgumentParser(description="Render the upstream ZGY DAG for Azure SPI")
    parser.add_argument("--source", type=Path, required=True)
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    render_file(args.source, args.output)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
