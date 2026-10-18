from datetime import timedelta

from osdu_airflow.backward_compatibility.default_args import \
    update_default_args

import airflow
from airflow import DAG
from airflow.contrib.operators.kubernetes_pod_operator import \
    KubernetesPodOperator

from osdu_airflow.operators.update_status import UpdateStatusOperator

# default args for airflow
default_args = {
    'owner': 'airflow',
    'depends_on_past': False,
    'start_date': airflow.utils.dates.days_ago(0),
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

# Get environment variables
env_vars = {
    "STORAGE_SVC_URL": "{| STORAGE_SVC_URL |}",
    "SD_SVC_URL": "{| SD_SVC_URL |}",
    "STORAGE_SVC_TOKEN": authorization,
    "STORAGE_SVC_API_KEY": storage_svc_api_key,
    "SD_SVC_API_KEY": sd_svc_api_key,    
    "OSDU_DATAPARTITIONID": dataPartitionId,
    "SD_READ_CACHE_PAGE_SIZE": "4195024",
    "SD_READ_CACHE_MAX_PAGES": "16",
    "SEGYTOZGY_VERBOSITY": "3",
    "SEGYTOZGY_GENERATE_INDEX": "1"
}
env_vars.update({| EXTRA_ENV_VARS|default('{}') |})


env_vars['SD_SVC_TOKEN'] = env_vars['STORAGE_SVC_TOKEN'] = 'Bearer ' + access_token

with DAG(
    DAG_NAME, 
    default_args=default_args,
    schedule_interval=None
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
