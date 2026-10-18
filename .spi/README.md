# Azure-native SPI packaging

This directory owns the Azure-native packaging layered onto the synchronized
OSDU SEG-Y to ZGY converter source.

## Artifacts

One source commit publishes three immutable images:

- `osdu-spi-zgy-converter`, the native `SegyToZgy` executable and its Azure
  authentication plugin;
- `osdu-spi-zgy-converter-dag`, a DAG-only layer consumed by the Stack-owned
  Airflow image; and
- `osdu-spi-zgy-converter-acceptance`, the Workflow registration check.

The native build enables `DefaultAzureCredential`, runs the converter unit
tests and bounded argument fuzzing, exports `/ctest.xml` through the
`spi-test` target, and copies the tested artifacts into a non-root runtime.
Its public OpenZGY and Azure SDK inputs are pinned in both the Dockerfile and
[PROVENANCE.md](./PROVENANCE.md).

## DAG contract

The upstream DAG remains unchanged. `ci/spi_zgy/render_dag.py` renders it for
SPI with:

- DAG ID `segy-to-zgy-conversion`;
- the `platform` namespace and `workload-identity-sa`;
- restricted pod and container security contexts;
- the `spi-pool: osdu` placement contract;
- in-cluster Storage and Seismic V3 endpoints;
- empty bearer-token variables so the conversion pod uses Workload Identity;
- `AIRFLOW_VAR_ZGY_CONVERTER_IMAGE` as the immutable converter image input;
  and
- `AIRFLOW_VAR_ZGY_CONVERTER_AUTH_SCOPE` as the deployment's
  `${AAD_CLIENT_ID}/.default` OSDU application scope.

The companion image contains only the rendered file under `/dags`. The SPI
Stack assembles that layer with other source-owned DAG layers into its one
Airflow control-plane image.

## Validation

The generated repository workflows run renderer tests, Ruff, exact Airflow
3.2.2 `DagBag` validation, native compilation and tests, both image builds,
the acceptance image build, and CodeQL. End-to-end certification belongs to
the Stack Borrow, Prove, Restore workflow because the DAG depends on live
Workflow, Storage, and Seismic services.
