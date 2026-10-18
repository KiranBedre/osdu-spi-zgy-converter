# Requires osdu-airflow~=0.30.0 (Airflow 2/3 backward_compatibility adapter).
from datetime import timedelta

import pendulum
from airflow import DAG

from osdu_airflow.backward_compatibility import (
    get_dag_schedule_kwargs,
    get_kubernetes_pod_operator,
    update_default_args,
)
from osdu_airflow.operators.update_status import UpdateStatusOperator

# Resolve version-specific KubernetesPodOperator via the compat adapter (AF2 + AF3)
KubernetesPodOperator = get_kubernetes_pod_operator()

# default args for airflow
# Fixed start_date: days_ago(0) is runtime-varying and makes Airflow 3 mint a new DAG version daily.
default_args = {
    'owner': 'airflow',
    'depends_on_past': False,
    'start_date': pendulum.datetime(2021, 1, 1),
    'retries': 0,
    'retry_delay': timedelta(minutes=5),
    'execution_timeout': timedelta(hours=24),
    'dagrun_timeout': timedelta(hours=24),
}

default_args = update_default_args(default_args)

# Get values from dag run configuration
authorization = "{{ dag_run.conf['authToken'] }}"
sd_svc_token = "{{ dag_run.conf['execution_context']['id_token'] or dag_run.conf['authToken'] }}"
dataPartitionId = "{{ dag_run.conf['execution_context']['data_partition_id'] }}"
run_id = "{{ dag_run.conf['runId'] }}"
sd_svc_api_key = "{{ dag_run.conf['execution_context']['sd_svc_api_key'] }}"
storage_svc_api_key = "{{ dag_run.conf['execution_context']['storage_svc_api_key'] }}"
filecollection_segy_id = "{{ dag_run.conf['execution_context']['filecollection_segy_id'] }}"
work_product_id = "{{ dag_run.conf['execution_context']['work_product_id'] }}"
svc_token = "{{ dag_run.conf['execution_context']['id_token'] or dag_run.conf['authToken'] }}"
user_id = "{{ dag_run.conf['execution_context'].get('userId') }}"
# Constants

DAG_NAME = "{| DAG_NAME |}" # Replace it by the workflowName passed on workflow service API.
DOCKER_IMAGE = "{| DOCKER_IMAGE |}" # Replace it by the segy-to-zgy container
SEGY_CONVERTER = "segy-to-zgy"
DAG_INIT = "dag-init"
NAMESPACE = "{| NAMESPACE |}"
K8S_POD_KWARGS = {|  K8S_POD_OPERATOR_KWARGS |}
if not K8S_POD_KWARGS:
    K8S_POD_KWARGS = {}

# Values to pass to csv parser
params = ["--osdu", filecollection_segy_id, work_product_id ]

#To enable Azure MSI, set the SD_SVC_TOKEN and STORAGE_SVC_TOKEN to empty strings. Add AZURE_ENABLE_MSI env var in EXTRA_ENV_VARS with value "true".

# Get environment variables
env_vars = {
    "STORAGE_SVC_URL": "{| STORAGE_SVC_URL |}",
    "SD_SVC_URL": "{| SD_SVC_URL |}",
    "SD_SVC_TOKEN": sd_svc_token,
    "STORAGE_SVC_TOKEN": authorization,
    "STORAGE_SVC_API_KEY": storage_svc_api_key,
    "SD_SVC_API_KEY": sd_svc_api_key,    
    "OSDU_DATAPARTITIONID": dataPartitionId,
    "SD_READ_CACHE_PAGE_SIZE": "4195024",
    "SD_READ_CACHE_MAX_PAGES": "16",
    "SEGYTOZGY_VERBOSITY": "3",
    "SEGYTOZGY_GENERATE_INDEX": "1",
    "USER_ID": user_id
}
env_vars.update({| EXTRA_ENV_VARS|default('{}') |})

with DAG(
    DAG_NAME,
    default_args=default_args,
    **get_dag_schedule_kwargs(None),
) as dag:
    update_status_running = UpdateStatusOperator(
        task_id="update_status_running",
    )

    segytozgy_converter = KubernetesPodOperator(
        namespace=NAMESPACE,
        task_id=SEGY_CONVERTER,
        name=SEGY_CONVERTER,
        env_vars=env_vars,
        arguments=params,
        is_delete_operator_pod=True,
        image=DOCKER_IMAGE,
        **K8S_POD_KWARGS
    )

    update_status_finished = UpdateStatusOperator(
        task_id="update_status_finished",
        trigger_rule="all_done"
    )

update_status_running >> segytozgy_converter >> update_status_finished # pylint: disable=pointless-statement
