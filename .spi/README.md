# Azure-native SPI packaging

This directory owns the Azure-native packaging layered onto the synchronized
OSDU SEG-Y to ZGY converter source.

## Artifacts

One source commit publishes three immutable images and one OCI artifact:

- `osdu-spi-zgy-converter`, the native `SegyToZgy` executable and its Azure
  authentication plugin;
- `osdu-spi-zgy-converter-dag-image`, a legacy DAG-only image retained as a
  separately named compatibility artifact;
- `osdu-spi-zgy-converter-acceptance`, the Workflow registration check.
- `osdu-spi-zgy-converter-dag`, the independently released `dag.zip`,
  checksum, and Workflow registration body consumed by the Stack.

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

The Stack pulls the digest-pinned OCI artifact, verifies its checksum, writes
the DAG ZIP to the shared Airflow Blob volume, and registers the artifact-owned
Workflow body. The generic Airflow runtime and native converter image remain
independently releasable.

## Validation

The generated repository workflows run renderer tests, Ruff, exact Airflow
3.2.2 `DagBag` validation, native compilation and tests, both image builds,
the acceptance image build, and CodeQL. End-to-end certification belongs to
the Stack Borrow, Prove, Restore workflow because the DAG depends on live
Workflow, Storage, and Seismic services.
