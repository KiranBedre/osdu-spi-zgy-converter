# Running the conversion

This documentation is focusing on conversion parameters which can be specified in the command line or when running as a Docker image.

The intended audience of this document are testers and service vendors.
In an OSDU environment, the conversions are usually orchestrated by the Workflow Service. In that case, many of these parameters will be hardcoded or managed by the DAG.

## Conversion process

### Overview

- read necessary records from storage service
- (optional) create SEG-Y index
- prepare OpenZGY output parameters
- read each brick from SEG-Y source, write each brick to ZGY output
- update records with reference to converted output

### Read necessary records from storage service

The converter will read the following records in order to locate the input dataset and get the necessary header mappings:

```
   |
   +-- SEG-Y file collection (ID specified on command line) [type: dataset--FileCollection.SEGY]
   |
   +-- Work product (ID specified on command line) [type: work-product--WorkProduct]
        |
        | ( references in: data.Components[] expanded below)
        |
        +-- Seismic trace data [type: work-product-component--SeismicTraceData]
        |
        +-- Seismic bin grid [type: work-product-component--SeismicBinGrid]
```

If any of these records is missing or invalid, the conversion will not start.

Note that it is technically possible to start conversion on a SEG-Y dataset which is not related to the work product, this is undefined behavior, and may lead to inconsistent/corrupted records.

### Create SEG-Y index

SEG-Y file format was originally designed for magnetic tapes, and only support sequential access.
An index makes it possible to read any trace of a SEG-Y dataset without having to read the whole "tape" first.
Unfortunately, building an index has a cost: the whole dataset must be read once.

If indexing needs to read the whole dataset and conversion also reads the whole dataset then why we don't just skip the indexing then?
Because SEG-Y files are not always regular, in some cases they can have gaps, can have different number of samples in each trace, the traces' storage order can be different, it is not trivial to figure out the geometry and the trace locations, and both of them are needed for a successful conversion.
So, if we don't index the dataset in advance then we would have to scan the whole dataset each time again (i.e., if we don't index we would have to create an index on each run and throw it away).

Indexing can be activated via environment variable. If indexing is enabled then the converter will create an index.
This occurs before the first conversion of the SEG-Y input file, and the index is saved back to SDMS for future use.

Conversion is relying on the index, if indexing is not enabled and there is no existing index for the dataset then the conversion can't proceed.

### Prepare OpenZGY output parameters

In this phase, we collect all the metadata required to create the output OpenZGY dataset.

Geometry, units, which are required to create the ZGY output, are extracted from the `work-product-component--SeismicTraceData` and `work-product-component--SeismicBinGrid` records, and the SEG-Y index.

Output file path will be generated from the input: insert a GUID after the file name, replace `.sgy` extension with `.zgy`.
A custom output file name can be specified via environment variable.

### Read each brick from SEG-Y source, write each brick to ZGY output

Bricks are small, regular 3-D sections of the seismic cube.
Their storage differs from the SEG-Y storage: bricks are close together each other in 3-D space, but SEG-Y traces are close together each other in a line.

Because SEG-Y traces must be read in an order different from the storage, collecting trace data for a brick involves several seeks ahead and backwards on the sequential "tape" structure of the SEG-Y file.
To avoid reading the same segment of the input dataset several times, reads are cached.
To achieve reasonably good throughput, data is read in larger chunks.

The cache size and chunk size are important factors in performance optimization and operational requirements (e.g., memory use of converter pods).

OpenZGY library has a separate caching/buffering mechanism for the output.

All relevant input and output parameters can be configured via environment variables.

### Update records with reference to converted output

The following records will be written:

```
   |
   +-- Seismic trace data (new version) [type: work-product-component--SeismicTraceData]
        |
        | (added reference in: data.Artefacts[], ArtefactRole:ConvertedContent)
        |
        +-- (new record) ZGY file collection [type: dataset--FileCollection.Slb.OpenZGY]
             |
             | (added reference in: ancestry.parents[])
             |
             +-- a reference to the original FileCollection.SEGY will be preserved
```

## Docker container - service dependencies and environment variables

### General

Data partition ID is required and used for all services: `OSDU_DATAPARTITIONID=my_data_partition`

Indexing: `SEGYTOZGY_GENERATE_INDEX(*)` Indexing is a prerequisite of the conversion (indexing collects the required geometry data from the SEG-Y trace headers). If the index does not exist, it will be created if `SEGYTOZGY_GENERATE_INDEX` is active, otherwise the conversion will fail.

Verbose output for debugging: `SEGYTOZGY_VERBOSITY(*)` and `SEGYTOZGY_INSECURE_PRINT_TOKEN(*)` Debug output (if active) is written to the standard output. Without verbosity there is practically no output from the conversion process, only an error message in case of failure. All debug output is prefixed with a timestamp relative to process start.

[!] Security concern: only use `SEGYTOZGY_INSECURE_PRINT_TOKEN(*)=1` while debugging potential token issues.

Variables marked with `(*)` should be set to `1` to activate or not set at all.

Overriding the output file name: if `SEGYTOZGY_OUTPUT_PATH` environment variable is set, the converted OpenZGY dataset will be saved with this name. If it already exists, it will be overwritten without notice.
If not specified, the input dataset path and a randomly generated suffix will be used as output.

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
- The read cache is divided to `page`s. For each read attempt, a whole page will be read from the cloud storage, and kept in the memory until the memory is needed for a more recent page read.
- `SD_READ_CACHE_PAGE_SIZE` is the number of bytes read from SD in a single read operation. `4195024` (4MB) is a safe default
- `SD_READ_CACHE_MAX_PAGES` max number of pages is the max number of pages. `64` 4MB pages are a safe default
    - crossline-ordered and non-linear input files may require more than the recommended default cache in some cases
- `SD_READ_CACHE_BLOCKS_PER_PAGE` the number of parallel I/O threads to fill the cache from the cloud storage, should normally be set to 64
    
Allocated memory for read cache will be: Number of inlines in a brick(64) x  number of traces per brick(64) * size of a single trace(e.g. 8240 for 2000 samples) * 2 * SD_READ_CACHE_MAX_PAGES 
                                        If SD_READ_CACHE_MAX_PAGES has a value of 16, we get :
                                         = 64 * 64 * 8240 * 2 * 16 = 1080033280 bytes = 1030MB
- Example and explanation of defaults:
    ```
    "SD_READ_CACHE_PAGE_SIZE": "1048576", (Azure 1048576, GCP 4194304)
    "SD_READ_CACHE_BLOCKS_PER_PAGE": "64",
    "SD_READ_CACHE_MAX_PAGES": "16",
    ```


### Fault tolerance

In case of transient errors, SDAPI has implemented automatic retries, and the conversion relies on that mechanism. OpenZGY is also using SDAPI as underlying storage, this means that both reading the input dataset and writing the converted dataset is covered by this automatic retry policy.
At the time of writing, no such retry mechanism is available for calling other services, reading or updating the storage records, requesting access tokens....

### Authentication methods

The following authentication methods are supported by the converter:
- JWT (Bearer token)
- OAuth 2.0 client credentials flow

#### Bearer token

Simple bearer token authentication: use the supplied token to connect to remote services.

Make sure that the token is valid for the duration of the entire conversion process.
You can specify different tokens for each service.

- Storage Service: `STORAGE_SVC_TOKEN=eyJ0eXAiO.......`
- Seismic DMS: `SD_SVC_TOKEN=eyJ0eXAiO.......`

All tokens will be prefixed with `Bearer: ` unless they already start with `Bearer: `.

#### Client credentials flow

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
  Cached token will be purged with this threshold before expiration, and a new one will be requested. If not specified, default value is 120 seconds (e.g., if access tokens are valid for one hour, a new one will be requested before the first service call after 58 minutes have passed)
  This is to ensure that there are reasonably low amounts of access token requests, and there is enough time left to complete an operation before the access token expires.

## Command line

### Command line:

To run a conversion in an OSDU environment, the first argument MUST be `--osdu`, then the IDs of the input records.
All other settings can be set via environment variables.

`Usage: path/to/SegyToZgy --osdu filecollection_segy_reference work_product_reference`

- `filecollection_segy_id` reference to a FileCollection.SEGY.1.0.0 object on the storage service
- `work_product_id` reference to a WorkProduct.1.0.0 object on the storage service

References should be references to a record ID (i.e., no version number should be specified).

### Example Docker command line:

`docker run --env-file d:\osdu\my_osdu.env --rm -it my-segytozgy-image-tag --osdu opendes:dataset--FileCollection.SEGY:<ID>: opendes:work-product--WorkProduct:<ID>:`

### Contents of Docker env-file

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

- To verify environment setup, environment variables are dumped to the standard output if `SEGYTOZGY_VERBOSITY=1` environment variable is set
- Error message `StatsGenerator::getVector : Unable to locate usable vector` is usually an indicator that the `VectorHeaderMapping` for inline and crossline index byte positions is incorrect.
- Memory requests and limits:
    - The conversion starts, but does not complete:
        - Make sure that the pod has enough memory (according to our measurements, for the default DAG settings, a memory request of about 12-16 GiB is recommended). Look for memory errors near the end of the Airflow log.
        - Indexing is never finished (`<input_dataset_name>.idx` was not created in SDMS):
            - for some SEG-Y datasets (especially, very large ones)
                - a temporary file is used to collect data during indexing, and it may fill up the temp directory of the executing VM
                - the memory usage during indexing can go quite high
                - in such cases, please file a bug report
    - The conversion job is started, but the actual conversion does not start at all: if the memory request of the pod is bigger than the maximum available VM available for pod execution, Kubernetes will never schedule that pod
- Conversion is not a single transaction. In some cases, unfinished operations may leave behind some artifacts:
    - if the ZGY was already created and/or written to, the stale ZGY file will remain on SDMS.
    - if updating the SeismicTraceData record fails, there will be a dangling ZGY file collection record left in the storage service
