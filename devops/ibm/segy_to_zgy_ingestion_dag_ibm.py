#  Licensed Materials - Property of IBM
#  (c) Copyright IBM Corp. 2020. All Rights Reserved.

from osdu_airflow.backward_compatibility.default_args import \
    update_default_args
import airflow
from airflow import DAG
from airflow.contrib.operators.kubernetes_pod_operator import KubernetesPodOperator
from datetime import timedelta
from json import dumps, loads
from airflow.models import Variable
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
record_id = "{{ dag_run.conf['runConfiguration']['id'] }}"
authorization = "{{ dag_run.conf['authToken'] }}"
sd_svc_token = "{{ dag_run.conf['execution_context']['id_token'] or dag_run.conf['authToken'] }}"
dataPartitionId = "{{ dag_run.conf['execution_context']['data_partition_id'] }}"
run_id = "{{ dag_run.conf['runId'] }}"
sd_svc_api_key = "{{ dag_run.conf['execution_context']['sd_svc_api_key'] }}"
storage_svc_api_key = "{{ dag_run.conf['execution_context']['storage_svc_api_key'] }}"
filecollection_segy_id = "{{ dag_run.conf['execution_context']['filecollection_segy_id'] }}"
work_product_id = "{{ dag_run.conf['execution_context']['work_product_id'] }}"
access_token = "{{ dag_run.conf['execution_context']['access_token'] }}"

# Constants

DAG_NAME = "sgy-to-zgy-pipeline"  # Replace it by the workflowName passed on workflow service API.
SEGY_CONVERTER = "segy-to-zgy"
DAG_INIT = "dag-init"

# Values to pass to csv parser
params = ["--osdu", filecollection_segy_id, work_product_id]

# Get environment variables
env_dict = Variable.get("sgy_to_zgy_env_var")
env_vars = loads(env_dict)

DOCKER_IMAGE = env_vars["SGY_TO_ZGY_DOCKER_IMAGE"]
NAMESPACE = env_vars["SGY_TO_ZGY_NAMESPACE"]

# set JWT
env_vars['SD_SVC_TOKEN'] = env_vars['STORAGE_SVC_TOKEN'] = 'Bearer ' + access_token

with DAG(DAG_NAME, default_args=default_args) as dag:
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
        get_logs=False
    )

    update_status_finished = UpdateStatusOperator(
        task_id="update_status_finished",
        trigger_rule="all_done"
    )

update_status_running >> segytozgy_converter >> update_status_finished  # pylint: disable=pointless-statement
