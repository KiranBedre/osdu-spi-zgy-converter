# What is the DAG and why is it important?

`DAG` is a concept of Apache Airflow, the underlying implementation of the Workflow service.
It serves as an integration of the SEG-Y to ZGY converter for the Workflow service.
The DAG must be registered in the Workflow service once, before first use.

# For end-users

Your vendor has already installed the DAG. There is nothing else to do.

# For OSDU vendors

Refer to Workflow service documentation about
- creating the workflow in the Workflow service instance
- installing the DAG to the Airflow deployment

Note that the DAG in this repository is provided only as documentation.
Each service provider should fine-tune the settings based on their environment, otherwise severe performance issues or instability may occur.

Take into consideration that the SEG-Y to ZGY converter can consume a significant amount of memory for internal buffers.
It is recommended to add a memory request setting to the default DAG, otherwise the conversions may randomly fail.

## Detailed installation process for the DAG

### Requirements

The [Airflow DAG](airflow/workflow-svc-v2/segy_to_zgy_ingestion_dag.py) has dependencies from [osdu-airflow-lib](https://community.opengroup.org/osdu/platform/data-flow/ingestion/osdu-airflow-lib) package for common operators and backward compatibility.

Install it in Airflow Environment:

```shell
pip install 'osdu-airflow~=0.30.0' --extra-index-url=https://community.opengroup.org/api/v4/projects/668/packages/pypi/simple
```

### For Azure this step can be skipped
The DAG must be registered in the Workflow Service using `POST /v1/workflow` API. The workflow name must match the [DAG_NAME]( https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/blob/master/airflow/segy_to_zgy_ingestion_dag.py#L32) and the [DAG_CONTENT]( https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/blob/master/airflow/segy_to_zgy_ingestion_dag.py) must be passed as string to the property `workflowDetailContent`. 

#### Curl request
```
curl --location --request POST 'https://{path}/api/v1/workflow' \
    --header 'Authorization: Bearer {token}' \
    --header 'data-partition-id: {data-partition-id}' \
    --header 'Content-Type: application/json' \
    --data-raw '{
        "workflowName": "{DAG_NAME}",
        "description": "string"
        "concurrentTaskRun": 1,
        "concurrentWorkflowRun": 1,
        "workflowDetailContent": "{DAG_CONTENT}"
    }'
```
#### Expected response body
```
{
  "workflowId": "REFHX05BTUU=",
  "workflowName": "DAG_NAME",
  "description": "workflow-description",
  "concurrentWorkflowRun": 1,
  "concurrentTaskRun": 1,
  "active": true,
  "createdBy": "some-user@some-company-cloud.com",
  "creationDate": 1614251571221,
  "version": 1
}
```
