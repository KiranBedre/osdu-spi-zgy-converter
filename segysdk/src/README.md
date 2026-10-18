# SEG-Y SDK 

The SEG-Y SDK is an OSDU contribution that facilitates the creation of SEG-Y applications to be integrated with Seismic DMS. It is currently composed by a cross-platform SEG-Y library and the SEG-Y to ZGY converter tool used in the OSDU ingestion workflow. 

The core part of the SEG-Y SDK is written in C++ and wrappers for other languages can be implemented on top of it. Currently, the C and Python wrappers are available, but they are still in the early prototype phase.

The public API for the SEG-Y SDK follow few design principles that must guide future contributions: 

- Use case driven
- Testable  
- Separation of concerns
- Fail early design

Currently the main usage of the SEG-Y SDK is the SEG-Y to ZGY conversion tool, however the SDK purpose is to facilitate any application to access of SEG-Y dataset on Seismic DMS, providing similar semantic of local dataset access. 

## Known limitations

- Only SEG-Y revision 0 and 1 are currently supported. 
- CI pipeline for OSDU Gitlab is missing.
- Ubuntu 20.04 is the only supported Linux version.
- C++ API
  - C++ API is only validated with the SEG-Y to ZGY conversion use.  
  - The way of configuring and creating of file accessors should be reworked (FileReader/WriterFactory, GenericFileManager).
- C API
  - It is an early prototype. It may be fundamentally changed or rewritten in the future.
  - The way of returning strings causes an effect similar to a memory leak, there is no way to free the returned strings.
- Python wrapper
  - It is an early prototype. It may be fundamentally changed or rewritten in the future.
  - Many features are missing from the Python wrapper (e.g., setting the cache size for a connection)
- Index (JSON and binary) format is not documented.
- Converter
  - Indexing and conversion should be separate processes to support the usage scenario where the SEG-Y file needs to be indexed to allow random access of the file but conversion to ZGY is not required.
  - Indexer supports multiple input files, but converter can't read the data from them.
- DAG
  - DAG is provided only for documentation purposes, it is not production ready.
- Performance
  - Currently only implemented multi-threaded read service for seismic sections following the main access direction of the SEG-Y file, this will normally be the inline direction.
    - For inline data a large number of read processes are launched reading a sequential section of the SEG-Y file, each reading an optimum amount of data based on data bucket size in the cloud storage.
    - For sections perpendicular to the organization of the SEG-Y file (or indeed arbitrary traverses) the situation is very different. Here, there will be potentially be 1000s of little read operations, each returning data for a single trace.
    - Depending on the minimum block-size of the cloud storage, this might cause multiple gigabytes of data to be read to return data for a single crossline.
    - The architectural challenge will be to implement a slick read process for small bits of data and enabling these to be called as independent processes.
- Multi files dataset
  - Although multiple seismic files are supported for a single logical data volume in the SEGY indexing process, the other data providers which will extract trace data for a requested seismic line (inline or crossline) will currently only support a single SEG-Y file for a logical data volume.
- Limited feedback of progress to caller module.
  - Currently there is no consistent way of producing status feedback to the caller to monitor progress on long running operations. We hope to implement a standard logging framework to provide this.

## Future development
There are several big areas identified for possible further development. The priority for each of these has not yet been decided.

### 2D Seismic loading
This would be functionality to Read a 2D SEG-Y file containing the trace data and ingest it with coordinate information taken from either trace coordinates or external navigation files.
Navigation data support will be implemented for trace header coordinates first. External navigation format expected to be supported include OGP P1/11, UKOOA P1/84, and P1/90 format and possibly also the original UKOOA P1/78 variant. We should also support a way of loading user defined file format for coordinate files where there is one line of data per CDP or SP.

### 2D and 3D Prestack data loading
Prestack data will require some additional features compared to post stack 2D and 3D data files. From an architectural point of view the main difference between pre stack and post stack data is that while post stack seismic data will have a single, stacked trace per CDP, pre stack data will have multiple traces per CDP (also referred to as pre-stack gather) and lately pre stack data has been processed with multiple gathers per CDP as well. In pre stack workflow, many more of the attributes in the SEG-Y trace headers will be used, so this data type will have to be able to define all required trace header locations.

### 3D External navigation data support
Currently 3D Seismic loading is done using coordinates from the trace headers. 3D navigation formats are now well defined, e.g. OGP P6/11 (Seismic bin grid data exchange format)
