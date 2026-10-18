# Quick Start instructions for work with SEGY -> Zgy parser (Pre-ship)

- [Quick Start instructions for work with SEGY -> Zgy parser (Pre-ship)](#quick-start-instructions-for-work-with-segy---zgy-parser-pre-ship)
  - [Prerequisites](#prerequisites)
    - [Required Entitlements groups](#required-entitlements-groups)
    - [Generate ID token](#generate-id-token)
    - [Install SDUTIL](#install-sdutil)
    - [Other issues](#other-issues)
  - [Segy -> Zgy conversion](#segy---zgy-conversion)
    - [Create Seismic Store's `tenant` and `subprojects`](#create-seismic-stores-tenant-and-subprojects)
    - [Get Segy-file](#get-segy-file)
    - [Upload Segy-file to Seismic Store from your machine using SDUTIL](#upload-segy-file-to-seismic-store-from-your-machine-using-sdutil)
    - [Ingest Manifest with SeismicTraceData WPC and Segy FileCollection records](#ingest-manifest-with-seismictracedata-wpc-and-segy-filecollection-records)
    - [Start Segy -> Zgy conversion workflow](#start-segy---zgy-conversion-workflow)
    - [Verification](#verification)

This guide is about how to run Segy->Zgy conversion Workflow on  Pre-ship.

## Prerequisites

### Required Entitlements groups

Be sure you are in the following Entitlements groups(GET: `{{entitlements_api_url}}/entitlements/v2/groups`):

- `users.datalake.admins`
- `app.trusted`
- `service.seistore.admin`
- `service.entitlements.user`
- `service.workflow.creator`
- `seistore.system.admin`

### Generate ID token

Seismic Store requires well-generated ID token.

Follow this guide to get API Credentials: <https://community.opengroup.org/osdu/documentation/-/wikis/Releases/R2.0/GCP/GCP-Operation/User-Mng/OpenID-Connect#how-to-get-api-credentials-using-gcp-openid-playgroud>

The most common issue with generating right ID token is using wrong **ClientID** and **ClientSecrets**. Contact our devopses to obtain the right ones.

### Install SDUTIL

SDUTIL is a command line Python utility designed to work easily with Seismic Store.

Follow the installation guide here: <https://community.opengroup.org/osdu/platform/domain-data-mgmt-services/seismic/seismic-dms-suite/seismic-store-sdutil#installation>

Replace the content of `seismic-store-sdutil/sdlib/config.yaml` with this:

```yaml
seistore:
  service: '{"google": {"defaultEnv":{"url": "<seismic_store_host>/api/v3", "appkey": ""}}}'
  url: '<seismic_store_host>/api/v3'
  cloud_provider: 'google'
  key: ''
  env: 'defaultEnv'
  auth-mode: 'JWT Token'
  ssl_verify: False
  APPKEY: ''
  APPKEY_NAME: ''
auth_provider:
  default: ''
gcp:
  empty: 'none'

```

Then, initialize configs:

```shell
python sdutil config init
```

And choose the first option:

```shell
[1] google

Select the cloud provider: 1
```

Then, you may skip inserting the google (defaultEnv) application key.

Now your SDUTIL is ready to use with JWT token (your ID token).

Example:

```shell
python sdutil stat sd://<your_tenant> --idtoken=$ID_token
```

### Other issues

You may find it useful to read this page: <https://community.opengroup.org/osdu/documentation/-/wikis/Releases/R2.0/GCP/GCP-Pre-Prod-Onboarding-Documentation>

## Segy -> Zgy conversion

### Create Seismic Store's `tenant` and `subprojects`

The seismic store uri is a string used to uniquely address a resource in the system and can be obtained by appending the prefix sd:// before the required resource path: `sd://<tenant>/<subproject>/<path>*/<dataset>`

Before you start uploading the file, you may create `tenant` and `subproject`.

If you want to use **already created** `tenant` and `subproject`, ask the owner (creator) of the `subroject` to add you to it:

```shell
curl --location --request PUT '<seismic_store_host>/api/v3/user' \
--header 'Authorization: Bearer <id_token>' \
--header 'Content-Type: application/json' \
--header 'x-api-key: test' \
--header 'appkey: test' \
--data-raw '{
 "email": "<your_email>@<domain>.com",
 "path": "sd://<tenant>/<subproject>",
 "group": "editor"
}'
```

This command will add you to required Entitlements groups to work with the concrete `subproject.`

Create the `tenant`:

```shell
curl --location -g --request POST '<seismic_store_host>/api/v3/tenant/<new-tenant>' \
--header 'Authorization: Bearer <id_token>' \
--header 'data-partition-id: <data-partition-id>' \
--header 'Content-Type: application/json' \
--data-raw '{
    "gcpid": "<pre-ship Google Cloud project ID>",
    "esd": "<data-partition-id>.<domain>.com",
    "default_acl": "data.default.owners@<data-partition-id>.<domain>.com"
}'
```

Then, create the `subproject`:

```shell
curl --location -g --request POST '<seismic_store_host>/api/v3/subproject/tenant/<new-tenant>/subproject/<subproject>' \
--header 'Authorization: Bearer <id_token>' \
--header 'Content-Type: application/json' \
--header 'ltag: <data-partition-id>-demo-legaltag' \
--header 'appkey: test' \
--header 'data-partition-id: <data-partition-id>' \
--data-raw '{

    "storage_class": "MULTI_REGIONAL",
    "storage_location": "US",
    "acl": {
        "owners": [
            "data.default.owners@<data-partition-id>.<domain>.com"
        ],
        "viewers": [
            "data.default.viewers@<data-partition-id>.<domain>.com"
        ]
    }
}'
```

### Get Segy-file

It is recommended to use [this guide](https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/blob/master/doc/testing.md#test-data) to obtain the correct Segy-files.

To pass the positive testing scenarios use `ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.segy` in the further steps.

### Upload Segy-file to Seismic Store from your machine using SDUTIL

After you installed SDUTIL you must have:

- all required groups in Entitlements;
- well-generated ID token;
- properly configured SDUTIL;
- created Seismic Store's `tenant` and `subproject` with access to it.

Copy the file from your local machine to Seismic Store:

```shell
python sdutil cp /path/to/local/segy/file sd://<tenant>/<subproject>/<some_path>/<file_name> --idtoken=$ID_TOKEN
```

From this point your Segy-file is available as a dataset in Seismic Store.

To get the dataset's short info:

```shell
python sdutil stat sd://<tenant>/<subproject>/<some_path>/<file_name> --idtoken=$ID_TOKEN
```

Sometimes it is required to unlock your uploaded dataset

```shell
python sdutil unlock sd://<tenant>/<subproject>/<path>/<file_name>  --idtoken=$ID_TOKEN
```

`sd://<tenant>/<subproject>/<some_path>/<file_name>` is URI to address your dataset. With this URI you will work in the next steps.

### Ingest Manifest with SeismicTraceData WPC and Segy FileCollection records

These records can be found [here](https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/tree/master/doc/sample-records/volve).

Also, follow [the guide](https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/blob/master/doc/testing.md#customizing-the-storage-records-for-test-data) of how to generate records.

After these records generated, replace the FileCollection's `FileSource` with `sd://<tenant>/<subproject>/<some_path>/<file_name>` of your Segy file on Seismic Store.

Then, these records must be stored on Storage Service. The records are not still consistent in terms of OSDU, so they can't be Ingested
using Manifest Based Ingestion, so you must send them via Storage Service `PUT` request.

```shell
curl --location --request PUT '<storage_service_url>/v2/records' \
--header 'Content-Type: application/json' \
--header 'data-partition-id: <data-partition-id>' \
--header 'Authorization: Bearer <access_token>' \
--data-raw '[
    {
      ...
      "kind": "osdu:wks:work-product--WorkProduct:1.0.0",
      ...
    },
    {
      ...
      "kind": "osdu:wks:work-product-component--SeismicTraceData:1.0.0"
      ...
    },
    {
      ...
      "kind": "osdu:wks:work-product-component--SeismicBinGrid:1.0.0",
      ...
    },
    {
      ...
      "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0",
      ...
    }
]'

```

**Note**: These records will work only with `ST10010ZC11_PZ_PSDM_KIRCH_FULL_T.MIG_FIN.POST_STACK.3D.JS-017536.segy` file.

### Start Segy -> Zgy conversion workflow

After you uploaded the file on Seismic Store and added corresponding records to Storage Service, you can start the conversion workflow:

```shell
curl --location --request POST 'https://<workflow-hsot>/v1/workflow/Segy_to_zgy_conversion/workflowRun' \
--header 'data-partition-id: <data-partition-id>' \
--header 'Authorization: Bearer <access_token>' \
--header 'Content-Type: application/json' \
--data-raw '{
    "executionContext": {
        "Payload": {
            "AppKey": "test-app",
            "data-partition-id": "<data-partition-id>"
        },
        "sd_svc_api_key": "<api_key>",
        "storage_svc_api_key": "<api_key>",
        "filecollection_segy_id": "<file_collection_id>:",
        "work_product_id": "<work_product_id>:",
        "id_token": "<id_token>",
        "data_partition_id": "<data-partition-id>"
    }
}'
```

The following fields:

| Property               | Type   | Description                                                                                                             |
|------------------------|--------|-------------------------------------------------------------------------------------------------------------------------|
| data_partition_id      | string | Data partition ID                                                                                                       |
| sd_svc_api_key         | string | AppKey or ApiKey used to access Seismic DMS. It can be a random string if the key is not required in the deployment     |
| storage_svc_api_key    | string | AppKey or ApiKey used to access Storage Service. It can be a random string if the key is not required in the deployment |
| filecollection_segy_id | string | Record id for the dataset--FileCollection.SEGY used on this run.                                                        |
| work_product_id        | string | Record id for the work-product—WorkProduct used on this run.                                                            |
| id_token               | string | ID Token to work with Seismic Store service |

### Verification

After the workflow executed, SeismicTraceData WPC is enriched with `Artefacts` field with the link to the `FileCollection.Slb.OpenZGY` record.

Verification is described [here](https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/blob/master/doc/testing.md#verification).
