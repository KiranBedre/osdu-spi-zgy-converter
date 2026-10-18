# Testing the conversion workflow

## Prerequisites

The conversion expects that
- The SEG-Y source file is already ingested into the Seismic DMS (SDMS)
- The following records are ingested into the Storage Service with correct references between each, and parameters customized to the SEG-Y file you aim to convert:
    - [dataset--FileCollection.SEGY](https://community.opengroup.org/osdu/data/data-definitions/-/blob/master/Examples/dataset/FileCollection.SEGY.1.0.0.json)
    - [work-product--WorkProduct](https://community.opengroup.org/osdu/data/data-definitions/-/blob/master/Examples/work-product/WorkProduct.1.0.0.json)
    - [work-product-component--SeismicTraceData](https://community.opengroup.org/osdu/data/data-definitions/-/blob/master/Examples/work-product-component/SeismicTraceData.1.0.0.json)
    - [work-product-component--SeismicBinGrid](https://community.opengroup.org/osdu/data/data-definitions/-/blob/master/Examples/work-product-component/SeismicBinGrid.1.0.0.json)

## Details

### Before everything else

This description will cover one way to set up all prerequisites for running a successful conversion workflow and validate the results. The process will require some manual steps, and it may feel clunky at times.

There can be simpler ways to accomplish the same task. For example, at the time of writing I was unable to verify the process via the ingestion service, which could maybe enable using surrogate keys for references between records.

### Test data

The record for test data are prepared for two seismic files from the [Volve dataset](https://www.equinor.com/en/what-we-do/digitalisation-in-our-dna/volve-field-data-village-download.html). Those two files should be downloaded to your computer to use the supplied JSON fie:
- ST10010ZC11_PZ_PSDM_KIRCH_FULL_D.MIG_FIN.POST_STACK.3D.JS-017536.segy
- ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.segy

Follow the dataset download instructions, and find them inside folder `Seismic/ST10010/Stacks`.

The sample records were meant to be similar to real-world data, significant part of their content is not directly related to conversion.

### Ingesting SEG-Y file into SDMS

Setting up SDMS and related tooling is outside the scope for this documentation. I assume that your deployment already has SDMS configured and enabled for you. Please contact the SDMS project team for assistance, if needed.

A convenient way to ingest the source file is the SDUTIL command line tool.

The commands needed for ingestion are:
- `sdutil cp <localpath> <sdpath>` to copy a local file to your SDMS location
- `sdutil ls <sdpath>` and `sdutil stat <sdpath>` to verify the upload
- `sdutil rm <sdpath>` to delete a partially uploaded file in case of an error
- `sdutil unlock <sdpath>` to unlock a file if SDUTIL could not finish an upload
- For other commands, please refer to SDUTIL [wiki page](https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/home/-/wikis/SDUTIL---Documentation).

Follow the Installation section of the wiki page.

`<localpath>` means any path accessible from your computer (can be network drive).

`<sdpath>` is a URL in form of `sd://<tenant>/<subproject>/path/file` where
- `tenant` is usually the same as data partition
- `subproject` can be an any name, maybe a business unit or a project name
- `path` is an arbitrary directory structure separated by `/`
- `file` is the file name

The full command to copy the file will look like `sdutil cp d:\my_downloads_folder\ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.segy sd://opendes/my-testing-subproject/volve/ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.segy`

### Customizing the storage records for test data

- Locate records in folder `sample-records/volve`
- Find both `FileCollection.SEGY.json` files, and correct the `"FileSource"` property to the `<sdpath>` used before.
- The sample JSON files assume that you have data partition ID `opendes` and schema authority name `osdu`. Further modifications will be needed to all JSON files if these differ on your system.

- Semi-automated method:
    - Use `prepare-records.sh` bash script
    - The script requires the `jq` command to be available on your system, this is usually not installed by default
    - Before running the script, open it in an editor, and review the settings, adapt to your deployment.
    - The output will be a JSON array with all objects
- Fully manual method:
    - Create proper ACL and legal sections in all JSON files
    - Way 1 - storage service generates the IDs
        - Create SeismicBinGrid record, save the ID
        - Create FileCollection.SEGY record, save the ID
        - Paste IDs into SeismicTraceData JSON
            - data.Datasets: array, only one element: ID of FileCollection.SEGY
            - data.BinGridId: string, ID of SeismicBinGrid
        - Create SeismicTraceData record, save the ID
        - Paste IDs into WorkProduct JSON
            - data.Components: array
                - ID of SeismicBinGrid
                - ID of SeismicTraceData
        - Create WorkProduct record, save the ID
    - Way 2 - pre-generated IDs
        - you can generate IDs for each object in advance, put them all into the correct places listed above, then send all objects to the storage service in one array

### Starting the conversion workflow

Once the DAG is registered the workflow can be triggered passing the workflow id and the proper payload.

### Payload fields

| Property               | Type   | Description                                                                                                             |
|------------------------|--------|-------------------------------------------------------------------------------------------------------------------------|
| data_partition_id      | string | Data partition ID                                                                                                       |
| sd_svc_api_key         | string | AppKey or ApiKey used to access Seismic DMS. It can be a random string if the key is not required in the deployment     |
| storage_svc_api_key    | string | AppKey or ApiKey used to access Storage Service. It can be a random string if the key is not required in the deployment |
| filecollection_segy_id | string | Record id for the dataset--FileCollection.SEGY used on this run.                                                        |
| work_product_id        | string | Record id for the work-product—WorkProduct used on this run.                                                            |

### Curl request example

#### Workflow service v1

```
curl --location --request POST 'https://{path}/api/workflow/v1/workflow/{workflow-id} \
    --header 'Authorization: Bearer {token}' \
    --header 'data-partition-id: {data-partition-id}' \
    --header 'Content-Type: application/json' \
    --data-raw '{
      "additionalProperties": {
            "sd_svc_api_key": "{api-key}",
            "storage_svc_api_key": "{api-key}",
            "filecollection_segy_id": "{record-id-from-storage}",
            "work_product_id": "{record-id-from-storage}"
          },
          "workflowTriggerConfig": {
                "id": "{record-id-from-storage}",
                "dataPartitionId": "{data-partition-id}",
                "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0"
            }
        }
    }'
```

#### Workflow service v2

```
curl --location --request POST 'https://{path}/api/workflow/v1/workflow/{workflow-id} \
    --header 'Authorization: Bearer {token}' \
    --header 'data-partition-id: {data-partition-id}' \
    --header 'Content-Type: application/json' \
    --data-raw '{
  "executionContext": {
    "data_partition_id": "{data-partition-id}",
    "sd_svc_api_key": "{api-key}",
    "storage_svc_api_key": "{api-key}",
    "filecollection_segy_id": "{record-id-from-storage}",
    "work_product_id": "{record-id-from-storage}"
  }
}'
```

### Expected response body

```
{
  "workflowId": "REFHX05BTUU=",
  "runId": "workflow-run-id",
  "startTimeStamp": 1614251794269,
  "status": "submitted",
  "submittedBy": "some-user@some-company-cloud.com"
}
```

### Verification

#### Updated records

- Fetch the seismic trace data record from storage service
    - The record will contain a new `Artefacts` entry:
        ```
        {
            "data": {
                "Artefacts": [
                    {
                        "ResourceID": "opendes:dataset--FileCollection.Slb.OpenZGY:b2ba80e968cd43b7a6a6f9ff6ad997b6:",
                        "ResourceKind": "osdu:wks:dataset--FileCollection.Slb.OpenZGY:1.0.0",
                        "RoleID": "opendes:reference-data--ArtefactRole:ConvertedContent:"
                    }
                ],
        [...]
        ```
    - This entry contains the ID of the newly created `FileCollection.Slb.OpenZGY` record, this contains the full path to the converted output.
        ```
        {
            "data": {
                "DatasetProperties": {
                    "FileSourceInfos": [
                        {
                            "FileSize": "1439694848",
                            "FileSource": "sd://opendes/my-testing-subproject/volve/ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.96279c2a-da9f-4da2-b3b1-97b348554b2b.zgy"
        [...]
        ```

#### Conversion output

The newly created dataset can be
- listed with `sdutil ls` and `sdutil stat`
- downloaded to local computer with `sdutil cp`
- examined in more detail with the OpenZGY library or any tool supporting the ZGY file format

Index files: index files are used to accelerate operations which would require full scanning of the SEG-Y file. When the conversion is run, it will create an `input_file.idx` and sometimes an `input_file.idx.bin` in the same folder as the input file.

Logs: All logging is done through stdout and stderr. Output is collected by Airflow.

### Troubleshooting

#### Monitoring the workflow run's status

- Workflow service endpoint `<workflow-svc-url>/workflow/<workflow-id>/workflowRun/<workflow-run-id>`
- Airflow web UI
    - process status
    - command line output and error log

#### Investigating, reporting errors

- Check converter output. If there is an error, there should be a human-readable error message near the end of the converter process' output
- Retry if there was a network or service error
- Check input file name and header mappings
- Open an issue
