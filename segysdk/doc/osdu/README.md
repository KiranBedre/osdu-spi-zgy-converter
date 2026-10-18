# SEGY to ZGY conversion

The SEGY to ZGY conversion is one step of the ingestion workflow. This conversion step is available as [Airflow DAG](https://airflow.apache.org/docs/apache-airflow/stable/concepts.html#dags) integrated with the Workflow Service. The DAG provided in this project calls a KubernetesPodOperator, which runs the container that is provided [here](https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/container_registry).

## FAQ

### Storage provider support

- Q: Does the converter support my cloud/storage provider: AWS, Azure, Google Cloud...?
- A: Not directly. The converter supports reading and writing seismic data via the Seismic DMS service and its client library (SDAPI). SDAPI supports AWS, Azure, Google Cloud at the time of writing this document, and can be extended for additional storage providers.

### Missing features

- Q: Is supporting {my provider, use case, new feature...} on your roadmap?
- A: Please start a conversation about your use case by opening an issue.

### Missing documentation

- Q: The documentation does not cover {important detail}
- A: Please open a merge request if you believe you are able to fill the documentation gap, or an issue if you need assistance.

## Prerequisites

Assembling all prerequisites for a successful conversion requires several steps, and may seem complicated at first. Read [testing instructions](doc/testing.md) carefully for detailed instructions.

## Registering DAG

See [DAG documentation](doc/DAG.md).

## Docker container and conversion process

See [Conversion documentation](doc/conversion.md).
