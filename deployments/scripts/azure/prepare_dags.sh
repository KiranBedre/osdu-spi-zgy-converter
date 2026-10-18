#!/usr/bin/env sh
#
#  Purpose: Create Dags
#  Usage:
#    prepare_dags.sh


###############################
## ARGUMENT INPUT            ##
###############################

usage() { echo "Usage: prepare_dags.sh"; }

printf "\n"
echo "------------------------------------";
echo "Creating Dags -- START ";
echo "------------------------------------";

if [ -z $NAMESPACE ]; then
  NAMESPACE=airflow
fi
export NAMESPACE

if [ -z $BUILD_VERSION ]; then
  BUILD_VERSION=$(echo ${CI_COMMIT_SHA} | cut -c -5)
fi
export BUILD_VERSION

if [ -z $DAG_TASK_IMAGE ]; then
  DAG_TASK_IMAGE=${CI_PROJECT_NAME}-${CI_COMMIT_REF_SLUG}:${CI_COMMIT_SHA}
fi
if [ ! -z $CI_COMMIT_TAG ]; then
  DAG_TASK_IMAGE=${CI_PROJECT_NAME}:${CI_COMMIT_SHA}
fi
export DAG_TASK_IMAGE

CI_PROJECT_NAME=$CI_PROJECT_NAME

export CI_PROJECT_NAME

echo "NAMESPACE: $NAMESPACE"
echo "BUILD_VERSION: $BUILD_VERSION"
echo "DAG_TASK_IMAGE: $DAG_TASK_IMAGE"

printf "\n"
echo "Execute Python Script output_dag_folder.py";
echo "------------------------------------";

python $AZURE_DEPLOYMENTS_SUBDIR/scripts/output_dag_folder.py -f $DAG_DIRECTORY/segy_to_zgy_ingestion_dag.py
