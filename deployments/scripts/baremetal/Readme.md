## BAREMETAL: DAG FILE RENDERING

```bash
# Install requirements
pip install Jinja2==2.10.1

# Setup Variables
export K8S_NAMESPACE="<your_k8s_namespace>"  # ie: default
export DATA_PARTITION="<your_partition>"   # ie:opendes
export STORAGE_SVC_URL=<storage_host>  # i.e. https://<storage_host>/api/storage/v2
export SD_SVC_URL=<ssdms_host>  # i.e https://<ssdms_host>/seistore-svc/api/v3

GC_DEPLOYMENTS_SUBDIR="<path_to_folder>" # ie: path to BAREMETAL deployment scripts folder
GC_DAG_DIRECTORY="<path_to_folder>" # ie: segy-to-zgy-conversion/airflow/workflow-svc-v2

# OUTPUT_FILE="<path_to_output_file>" # ie. to avoid original file overwriting

chmod +x bootstrap.sh
./bootstrap.sh
```

Workflow Payload Example:

{
    "executionContext": {
        "Payload": {
            "AppKey": "test-app",
            "data-partition-id": "{{data-partition-id}}"
        },
        "sd_svc_api_key": "{api-key}",
        "storage_svc_api_key": "{api-key}",
        "filecollection_segy_id": "{file_record_id}",
        "work_product_id": "{wp_record_id}",
        "data_partition_id": "{data-partition-id}"
    }
}
