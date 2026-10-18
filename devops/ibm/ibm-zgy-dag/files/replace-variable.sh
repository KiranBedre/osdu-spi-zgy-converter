#!/bin/bash
echo "Changing Source Dags"
sed -i '/from datetime import timedelta/ifrom json import dumps, loads' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed -i '/from datetime import timedelta/ifrom airflow.models import Variable' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed -i "/DAG_NAME = \"{| DAG_NAME |}\"/iaccess_token = \"{{ dag_run.conf['execution_context']['access_token'] }}\"" /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed -i 's/"{| DAG_NAME |}"/"sgy-to-zgy"/g' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed -i 's/"{| DOCKER_IMAGE |}"/Variable.get("SGY_TO_ZGY_DOCKER_IMAGE")/g' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed -i 's/"{| NAMESPACE |}"/Variable.get("SGY_TO_ZGY_NAMESPACE")/g' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed -i 's/{|  K8S_POD_OPERATOR_KWARGS |}/{}/g' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i 's/^env_vars = {/env_vars = loads(env_dict)/g' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i '/^    "STORAGE_SVC_URL/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py  
sed  -i '/^    "SD_SVC_URL"/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
#sed  -i '/^    "SD_SVC_TOKEN"/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i '/^    "STORAGE_SVC_API_KEY"/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i '/^    "SD_SVC_API_KEY"/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i '/^    "OSDU_DATAPARTITIONID"/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i '/^    "SD_READ_CACHE_PAGE_SIZE"/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i '/^    "SD_READ_CACHE_MAX_PAGES"/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i '/^    "SEGYTOZGY_VERBOSITY"/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i '/^    "STORAGE_SVC_TOKEN"/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i '/^    "SEGYTOZGY_GENERATE_INDEX"/,+1d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed -i '/**K8S_POD_KWARGS/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed -i '/env_vars.update/d' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed  -i 's/^        image=DOCKER_IMAGE,/        image=DOCKER_IMAGE/g' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
sed -i '/env_vars = loads(env_dict)/ienv_dict = Variable.get("sgy_to_zgy_env_var")' /opt/airflow/dags/segy_to_zgy_ingestion_dag.py
echo "ZGY DAGS Changed"
ls -ltr /opt/airflow/dags