# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]
### Added
### Changed
### Deprecated
### Removed
### Fixed
### Security

## [0.4.0] - 2023-01-26
### NOTE: 
The environment variable SD_READ_CACHE_MAX_PAGES now has a different use to before. The current implementation uses it to set the number of read-ahead buffers to use.
Each read-ahead buffer is holding 64 seismic traces, so a value of 1 specifies a read-ahead buffer of 64 traces, a value of 2 specifies a read-ahead buffer of 2 x 64 traces and so on.
For the conversion under default settings there will be 64 readers, each having their own read-ahead buffer of the specified size, and there are 2 banks of these 64 readers.
We would normally expect a value of between 8 and 64 for the SD_READ_CACHE_MAX_PAGES. For a value of 8, the memory requirement for the read-cache will be ~540MB assuming 2000 samples.
A value of 16 uses slightly overr 1GB of RAM for the cache buffer.
In our tests we have seen that a value of 16 allows the DAG to run in a Kubernetes cluster with 32GB RAM. 
per trace in the SEG-Y file.
### Added
- Improved performance for SEG-Y to ZGY converter through heavy use of multi-threading. 
- Support for GSM logging
- Refactored authentication module so new authentication methods e.g. oauth2, 
  can be provided through plugin. (See src/AuthPlugin for implementation details)
- 
### Fixed
- Several minor bug fixes including handling of versions in OSDU metadata files

## [0.4.0] - 2022-07-25
### Added
- SEG-Y access multithreaded [inline, crossline, bricked access for SEG-Y to ZGY converter]
- improved SEG-Y index to support decreasing inlines/crosslines increments
- added sample code to demonstrate how to use the functionality of SEG-Y SDK API
- allow specification ZGY output format from SEG-Y to ZGY converter
- support for handling of SEG-Y files with missing inlines/crosslines
### Fixed
- invalid format for OSDU metadata after ZGY conversion
- fixed memory leaks in SEG-Y to ZGY converter
- done a lot of refactoring work and general code improvements, performance enhancements

## [0.1.4] - 2021-07-30
### Changed
- significantly reduced amount of log written during conversion
- added progress report during "Closing ZGY" phase
### Fixed
- allow conversion if schema authority is not "osdu"

## [0.1.3] - 2021-06-09
### Changed
- updated SDMS client library
- more specific log message for `Unable to determine byte order, unrecognized sample format.` error
### Fixed
- correct handling of unitless "unit" `Euc`

## [0.1.2] - 2021-05-10
### Fixed
- fixed handling of vertical unit in trace header mapping
- fixed conversion performance issue

## [0.1.1] - 2021-04-16
### Changed
- use internal parser to get unit from bin grid
### Removed
- removed settings and documentation related to Spatial Reference Service
### Fixed
- fixed corrupted exception messages containing HTTP status code

## [0.1.0] - 2021-03-05
### Changed
- changed output file naming to include a GUID (to avoid overwrites)
### Fixed
- fixed/revised update of records after successful conversion

## [0.0.0] - 2021-02-26
### Added
- initial release of binaries

[Unreleased]: https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/compare/v0.1.4...master
[0.1.4]: https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/compare/v0.1.3...v0.1.4
[0.1.3]: https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/compare/v0.1.2...v0.1.3
[0.1.2]: https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/compare/v0.1.1...v0.1.2
[0.1.1]: https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/compare/v0.1.0...v0.1.1
[0.1.0]: https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/compare/v0.0.0...v0.1.0
[0.0.0]: https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/tags/v0.0.0
