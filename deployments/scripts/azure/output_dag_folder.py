import json
import os
import argparse
import shutil
from jinja2 import Environment, FileSystemLoader


class CreateDag:

    def __init__(self, file_path):
        self.dag_folder_path = os.environ.get(
            'ZGY_DAG_FOLDER_PATH', '/home/osdu/airflow/workflow-svc-v2')
        self.dag_task_image = os.environ.get("DAG_TASK_IMAGE")
        self.dns_host_name = "{{ var.value.azure_dns_host }}"
        self.namespace = os.environ.get('NAMESPACE')
        self.file_path = file_path
        self.service_name = "segy-to-zgy-conversion"
        self.build_version = os.environ.get('BUILD_VERSION', 'latest')
        self.dag_name = f"{self.service_name}"
        self.dag_name = self.dag_name[:50]
        self.ci_project_name = os.environ.get("CI_PROJECT_NAME")

        self.env_vars = {
            "STORAGE_SVC_URL": f"https://{self.dns_host_name}/api/storage/v2/",
            "STORAGE_SVC_TOKEN": "{{ dag_run.conf['execution_context']['access_token'] }}",
            "SD_SVC_URL": f"https://{self.dns_host_name}/seistore-svc/api/v3",
            "SD_SVC_TOKEN": "{{ dag_run.conf['execution_context']['access_token'] }}"
        }

        self.enable_azure_aad_vars = os.environ.get(
            'AZURE_AAD_VARIABLES', 'true')
        if self.enable_azure_aad_vars == 'true':
            self.env_vars["AZURE_TENANT_ID"] = "{{ var.value.azure_tenant_id }}"
            self.env_vars["AZURE_CLIENT_ID"] = "{{ var.value.azure_client_id }}"
            self.env_vars["AZURE_CLIENT_SECRET"] = "{{ var.value.azure_client_secret }}"
            self.env_vars["aad_client_id"] = "{{ var.value.aad_client_id }}"

        self.kubernetes_pod_operator_options = {
            "labels": {
                "aadpodidbinding": os.environ.get('POD_IDENTITY_TAG', "osdu-identity")
            },
            "annotations": {
                "sidecar.istio.io/inject": "false"
            }
        }

        print(f"file_path: {self.file_path}")
        print(f"service_name: {self.service_name}")
        print(f"build_version: {self.build_version}")
        print(f"dag_name: {self.dag_name}")
        print(f"folder: {self.dag_folder_path}")
        print(f"dag_task_image: {self.dag_task_image}")

    def _render_template(self, file) -> str:
        env = Environment(
            loader=FileSystemLoader(searchpath=os.getenv(
                "ZGY_DAG_FOLDER_PATH", default="/home/osdu/airflow/workflow-svc-v2")),
            variable_start_string='{|', variable_end_string='|}',
        )
        template = env.get_template(os.path.basename(file.name))
        
        params = {
            "DAG_NAME": self.dag_name,
            "DOCKER_IMAGE":  "{{ var.value.dag_image_acr }}/" + self.ci_project_name + ":{{ var.value." + self.ci_project_name.replace("-","_") + "_image_tag }}", 
            "NAMESPACE": self.namespace,
            "K8S_POD_OPERATOR_KWARGS": json.dumps(self.kubernetes_pod_operator_options, indent=2),
            "EXTRA_ENV_VARS": json.dumps(self.env_vars, indent=4),
        }

        return template.render(**params)

    def create(self):
        self.read_dag_and_replace_placeholders()

    def read_dag_and_replace_placeholders(self):
        with open(self.file_path, 'r', encoding='utf-8') as f:
            data = self._render_template(f)
            current_directory = os.getcwd()
            final_directory = os.path.join(
                current_directory, r'output_dags/temp_folder')
            if not os.path.exists(final_directory):
                os.makedirs(final_directory)
            new_file = open("./output_dags/temp_folder/" +
                            self.dag_name + ".py", "w")
            new_file.write(data)
            new_file.close()
            self.create_workflow_request_body()
            self.copy_zgy_converter_folder()
            self.prepare_zip_folder()

    def prepare_zip_folder(self):
        shutil.make_archive('./output_dags/dags/zgy_converter',
                            'zip', './output_dags/temp_folder')
        shutil.rmtree('./output_dags/temp_folder')

    def copy_zgy_converter_folder(self):
        shutil.copytree(self.dag_folder_path,
                        './output_dags/temp_folder/osdu_zgy_converter')

    def create_workflow_request_body(self):
        file_name = os.environ.get(
            "WORKFLOW_REQUEST_BODY_FILE", "workflow_request_body.json")
        new_file = open("./output_dags/" + file_name, "w")
        data = '[{"workflowName":"DAG_NAME_PLACEHOLDER","description":"ZGYConverter","registrationInstructions":{"dagName":"DAG_NAME_PLACEHOLDER"}}]'
        data = data.replace("DAG_NAME_PLACEHOLDER", self.dag_name)
        new_file.write(data)


if __name__ == "__main__":

    parser = argparse.ArgumentParser(
        description="Register python dag file to the workflow service.")

    parser.add_argument('-f', type=str,
                        help='The path to the load python dag file.',
                        default=None)

    arguments = parser.parse_args()

    if arguments.f is not None:
        file_path = arguments.f

    CreateDag(file_path).create()
