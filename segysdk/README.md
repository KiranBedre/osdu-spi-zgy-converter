# SEGY to ZGY conversion

The SEGY to ZGY conversion is one step of the ingestion workflow. This conversion step is available as [Airflow DAG](https://airflow.apache.org/docs/apache-airflow/stable/concepts.html#dags) integrated with the Workflow Service. The DAG provided in this project calls a KubernetesPodOperator, which runs the container that is provided [here](https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/container_registry).

## FAQ

### Storage provider support

- Q: Does the converter support my cloud/storage provider: AWS, Azure, Google Cloud...?
- A: Not directly. The converter supports reading and writing seismic data via the Seismic DMS service and its client library (SDAPI). SDAPI supports AWS, Azure, Google Cloud at the time of writing this document, and can be extended for additional storage providers.

### Missing features

- Q: Is supporting {my provider, use case, new feature...} on your roadmap?
- A: Please start a conversation about your use case by opening an issue.

### Missing documentation

- Q: The documentation does not cover {important detail}
- A: Please open a merge request if you believe you are able to fill the documentation gap, or an issue if you need assistance.

## Prerequisites

Assembling all prerequisites for a successful conversion requires several steps, and may seem complicated at first. Read [testing instructions](doc/testing.md) carefully for detailed instructions.

## Registering DAG

#### For Azure this step can be skipped

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

## Docker container - overview

### Conversion process

- read necessary records from storage service
- (optional) create SEG-Y index
- prepare OpenZGY output parameters
- read each brick from SEG-Y source, write each brick to ZGY output
- update records with reference to converted output

### Read necessary records from storage service

The converter will read the following records:

```
   |
   +-- SEG-Y file collection (ID specified on command line) [type: dataset--FileCollection.SEGY]
   |
   +-- Work product (ID specified on command line) [type: work-product--WorkProduct]
        |
        | ( references in: data.Components[] expaned below)
        |
        +-- Seismic trace data [type: work-product-component--SeismicTraceData]
        |
        +-- Seismic bin grid [type: work-product-component--SeismicBinGrid]
```

### Create SEG-Y index

If indexing is enabled, it occurs before first conversion of the SEG-Y input file and the result is saved back to SDMS for future use. The whole file is read during this process.

### Prepare OpenZGY output parameters

Geometry, units, which are required to create the ZGY output, are extracted from the `work-product-component--SeismicTraceData` and `work-product-component--SeismicBinGrid`.

Output file path will be generated from the input: insert a GUID after the file name, replace `.sgy` extension with `.zgy`. To override the default generated name, set environment variable `SEGYTOZGY_OUTPUT_PATH`.

Output will be overwritten if exists.

### Read each brick from SEG-Y source, write each brick to ZGY output

Reads are done in `SD_READ_CACHE_PAGE_SIZE` sized blocks, writes are handled by OpenZGY library.

### Update records with reference to converted output

The following records will be written:

```
   |
   +-- Seismic trace data (new version) [type: work-product-component--SeismicTraceData]
        |
        | (added reference in: data.Artefacts[], ArtefactRole:ConvertedContent)
        |
        +-- (new record) ZGY file collection [type: dataset--FileCollection.Slb.OpenZGY]
```

## Docker container - service dependencies and environment variables

### General

Data partition ID is required and used for all services: `OSDU_DATAPARTITIONID=my_data_partition`

Indexing: `SEGYTOZGY_GENERATE_INDEX(*)` Indexing is a prerequisite of the conversion (indexing collects the required geometry data from the SEG-Y trace headers). If the index does not exist, it will be created if `SEGYTOZGY_GENERATE_INDEX` is active, otherwise the conversion will fail.

Verbose output for debugging: `SEGYTOZGY_VERBOSITY(*)` and `SEGYTOZGY_INSECURE_PRINT_TOKEN(*)` Debug output (if active) is written to stdout. Without verbosity there is practically no output from the conversion process, only an error message in case of failure. All debug output is prefixed with a timestamp relative to process start.

[!] Security concern: only use `SEGYTOZGY_INSECURE_PRINT_TOKEN(*)=1` while debugging potential token issues.

Variables marked with `(*)` should be set to `1` to activate or not set at all.

### Storage Service

Storage service URL and authentication variables (depending on authentication method, detailed below) are required:

```
STORAGE_SVC_URL=https://my-osdu-api-gateway/api/storage/v2/
```

The converter will read and update records in the storage service.

[!] Concurrency: data races may occur if multiple conversions related to the same `SeismicTraceData` work product component run at the same time: references to one or more converted file collections may be missing after the record update.

### Seismic DMS (a.k.a. SDMS, SDAPI)

```
SD_SVC_URL=https://my-osdu-api-gateway/seistore-svc/api/v3
SD_READ_CACHE_PAGE_SIZE=<see below>
SD_READ_CACHE_MAX_PAGES=<see below>
```

`SD_SVC_URL` is not tolerant to trailing `/`

Cache settings:

- `SD_READ_CACHE_PAGE_SIZE` is the number of bytes read from SD in a single read operation. `4195024` (4MB) is a safe default
- `SD_READ_CACHE_MAX_PAGES` max number of pages is the max number of pages. `8` or 16 pages are usually a safe default (unless seismic trace has >> 2000 samples)
  - crossline-ordered and non-linear input files may require than the recommended default cache in some cases

### Authentication flows

The following authentication methods are supported:

#### Bearer token

Simple bearer token authentication: use the supplied token to connect to remote services.

Make sure that the token is valid for the duration of the entire conversion process.
You can specify different tokens for each service.

- Storage Service: `STORAGE_SVC_TOKEN=eyJ0eXAiO.......`
- Seismic DMS: `SD_SVC_TOKEN=eyJ0eXAiO.......`

All tokens will be prefixed with `Bearer:` unless they already start with `Bearer:`.

#### Client credentials

Typically for non-interactive services. Acquires access tokens for each called service from the OAuth token service endpoint.

Required environment variables:

- Client ID: `SEGYTOZGY_CLIENT_ID=...`
- Client secret: `SEGYTOZGY_CLIENT_SECRET=***` - note: you should handle this variable as a secret
- URL of the token service endpoint: `TOKEN_SVC_URL=https://auth.example.com/v2/token`
- Client ID of the called services, used for generating access tokens specific to the called service:
  - Storage Service: `STORAGE_SVC_SCOPE=...`
  - Seismic DMS: `SD_SVC_SCOPE=...`

Optional:

- `TOKEN_EXPIRATION_THRESHOLD_SECONDS`: By default, access tokens are cached in memory.
  Cached token will be purged with this threshold before expiration, and a new one will be requested. If not specified, default value is 120 seconds.
  This is to ensure that there are reasonable amounts of access token requests, and there is enough time left to complete an operation before the access token expires.

## Command line

### Command line

First argument MUST be `--osdu`.

`Usage: path/to/SegyToZgy --osdu filecollection_segy_reference work_product_reference`

- `filecollection_segy_id` reference to a FileCollection.SEGY.1.0.0 object on the storage service
- `work_product_id` reference to a WorkProduct.1.0.0 object on the storage service

References should be latest version references (no version number specified).

### Example Docker command line

`docker run --env-file d:\osdu\my_osdu.env --rm -it my-segytozgy-image-tag --osdu opendes:dataset--FileCollection.SEGY:<ID>: opendes:work-product--WorkProduct:<ID>:`

### Docker Env-file

Minimum required environment variables (see above for values)

```
OSDU_DATAPARTITIONID=...
STORAGE_SVC_TOKEN=...
STORAGE_SVC_URL=...
SD_SVC_URL=...
SD_SVC_TOKEN=...
SD_READ_CACHE_MAX_PAGES=...
SD_READ_CACHE_PAGE_SIZE=...
```

## Troubleshooting

- To verify environment setup, environment variables are dumped to stdout if `SEGYTOZGY_VERBOSITY=1` is set
- Error message `StatsGenerator::getVector : Unable to locate usable vector` is usually an indicator that the `VectorHeaderMapping` for inline and crossline index byte positions is incorrect.
- In case of an error
  - if the ZGY was already created and/or written to, the stale ZGY file will remain on SDMS.
  - if updating the SeismicTraceData record fails, there will be a dangling ZGY file collection record left in the storage service
