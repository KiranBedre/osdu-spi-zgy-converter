# SegySdk Programmer Guide
### Abstract
The present SegySdk Programmer Guide (later Guide) describes contents of the source code (later SegySdk folder) which can be obtained by the following link:
```
https://community.opengroup.org/osdu/platform/data-flow/ingestion/segy-to-zgy-conversion/-/tree/master/segysdk
```
The Guide describes the following topics:
* source code structure
* recommendation how to develop the `segysdk` shared library 
* expected binaries output

> **Note:** Before starting the SegySdk development please read the present Guide carefully and contact the SegySdk mainainers for more detailed information

## 1. Source Code Structure
### 1.1. Folder Structure
The SegySdk folder contains the following folders:
* `'doc'` - contains some documentation, including the present Guide
* `'node-wrapper'` - implements the **Node.JS** wrapper on the top of the SegySdk C language API interface
* `'python-wrapper'` - implements the **Python** wrapper on the top of the SegySdk C language API interface
* `'src'` - implements the **C++ SegySdk**, unit tests, tools, **C language API interface**, converter, and indexer
  * `'cmake'` - custom auxiliary functions for CMake utility
  * `'BulkAccess'` - implements SEG-Y files bulk readers
  * `'Concurrent'` - primitives for safe multi-threading
  * `'FileIO'` - implements various generic file accessors' entities and factories: direct, buffered, SD storage aware, sequential, sectional, etc
  * `'LibraryLoader'` - implements helper classes to load shared objects during runtime
  * `'Logger'` - implements multi-threading logger with support of adding new entities that write logs to different destinations
  * `'SegyFormat'` - implements SEG-Y file descriptive contents readers and corresponding string resources
  * `'SegyIndex'` - implements SEG-Y file parsing, indexing and browsing helpers, binary and JSON index file reading and generation
  * `'SegyIO'` - implements SEG-Y file readers that are aware of the SEG-Y file structure items such as file headers (textual and binary), trace headers and sample values 
  * `'segysdk'` - implements SEG-Y session entity and factory that are intended to keep the main context while operating on the SEG-Y files 
  * `'segysdk_details'` - keeps some entities that are used in common
  * `'segysdk_public'` - mainly implements the **C language API** and produces the `'segysdk'` shared library as the binary output
  * `'tests'` - implements a number of the unit tests to cover and keep consistent the SegySdk framework source code
  * `'tools'` - implements the based on the SegySdk framework applications and code samples
    * `'SampleCode'` - contains the SegySdk C++ framework usage examples and simple sample applications
    * `'SegyIndexerService'` - implements the SEG-Y file indexer application
    * `'SegyToZgy'` - implements the SEG-Y to ZGY convertor application
  * `'Utilities'` - implements some generic procedures and classes the SegySdk code depends on 
* `'testdata'` - file resources intended to support various unit tests as the input data and/or the reference output data
### 1.2. Source Code Modules Dependencies
According to the folder structure description above, it is possible to compose a simplified diagram of source code modules/folders dependencies.

The arrow means "depends on", so the modules to the left depend on those to the right.
> **Note:** The diagram below is not precise and is given for better understanding of internal code structure and dependencies
```
  - - - - - - - - - -                            - - - - - - - - - - - - - - - -
| SampleCode         |     - - - - - - - -      | BulkAccess, SegyIndex,        |
| SegyToZgy          | -> | segysdk_public | -> | Concurrent,  SegyIO,          |      - - - - - - - - - - - - 
| SegyIndexerService |     = = = = = = - -      | FileIO,     segysdk,          |  -> | third party frameworks |
| Unit Tests         | -> | testdata |          | SegyFormat, segysdk_details,  |      - - - - - - - - - - - - 
| node-wrapper       |     = = = = = - -        | Utilities, Logger,            |
|                    |                          | LibraryLoader                 |
| python-wrapper     | -> | segy.testing |       - - - - - - - - - - - - - - - -
 - - - - - - - - - -       - - - - - - - 
```
## 2. Shared Library Development (`segysdk`)
### 2.1. Purpose of `segysdk` Shared Library
The shared library stands as a SegySdk API entry point. 

All the executable or library binaries that rely on SegySdk API should use the SegySdk headers and link to the `segysdk` shared library 
### 2.2. `segysdk` Shared Library Dependency Structure
The shared library depends on the SegySdk framework components and some third party frameworks and libraries. 
#### 2.2.1. Components of the `segysdk` Shared Library
The SegySdk groups the framework components by source code folders and C++ namespaces.
##### 2.2.1.1. Component `CMakeLists.txt` File Requirements and Assumptions
Each SegySdk component groups own files under a separate folder and has own `CMakeLists.txt` file where the last one satisfies listed below requirements and assumptions.

Component `CMakeLists.txt` file assumptions:
- all include directories are declared using `PRIVATE` modifier; this is important for both of own/local and third party headers, otherwise the directories' information might be implicitly shared causing side effects
- all files are listed explicitly, including header and implementation files
- if the component depends on some external/third party component(s) then the dependency is not listed here

Component `CMakeLists.txt` file requirements:
- component library is marked as `STATIC`; all the components are linked as a static library but the main component composes all them to a shared library
- `SEGYSDK_BUILDING_DLL` compile definition is declared using `PRIVATE`, otherwise the definition might be implicitly shared causing side effects

Below is shown a typical `my_component` component's `CMakeLists.txt` file:
```
# Typical CMakeLists.txt file snippent for the component

add_library(my_component STATIC 
	# list components' files here
)

target_include_directories(my_component
PRIVATE
	# list include directories here including third party ones
)

target_compile_definitions(my_component
PRIVATE
	SEGYSDK_BUILDING_DLL 
)
```
##### 2.2.1.2. How to Export Entity
If some entity is to be in the shared `segysdk` library exported entities list then the following steps to be done:
- include `DLL_EXPORT.h` header file to the entity header file
- insert the `SEGYSDK_DLL_PUBLIC` macro modifier to the entity declaration
- avoid any inline member function bodies declarations
- avoid modern C++ syntax sugar like `=default`, for any members, including constructors, member functions, and destructors

Some examples of exported entities declaration are shown below:
```
// Example of exported entities declaration

#include <segyskd_details/DLL_EXPORT.h>

namespace my_entity_namespace
{

enum class SEGYSDK_DLL_PUBLIC MyEnum
{
    Value_01,
    Value_02 = 200
};

class SEGYSDK_DLL_PUBLIC MyEntityClass
{
public:
   MyEntityClass(); // DON'T USE =default statements
   void myFunction(); // DON'T USE inline bodies
};

SEGYSDK_DLL_PUBLIC bool MyFunction(int a, int b);

} // namespace my_entity_namespace
```
##### 2.2.1.3. `DLL_EXPORT.h` Header File
The `DLL_EXPORT.h` file provides the export/import decoration that compiler reads and generated the corresponding symboles list.

For SegySdk the `SEGYSDK_DLL_PUBLIC` macro is expanded to `__declspec(dllimport)` or `__attribute__((dllimport))` depending on `SEGYSDK_BUILDING_DLL` macro declaration presence.

> **Note:** The only `SEGYSDK_DLL_PUBLIC` macro allows creation of the single shared library. 
> 
> To create more than one shared library there are options described below:
> - implement some new independent `SHARED` library, i.e. which does not depend on other `SHARED` libraries created using `SEGYSDK_DLL_PUBLIC` macro and `DLL_EXPORT.h` file
> - create new `SHARED` library where all the dependency are `STATIC` libraries, e.g. the existing `segysdk` components
> - create an additional macro and the corresponding 'export' header file per each new `SHARED` library when there are some dependencies on e.g. `segysdk` `SHARED` library
#### 2.2.2. `segysdk` Shared Library
The `segysdk` shared library consists of several components and stands as a 'front man' for the entire SegySdk API. 
##### 2.2.2.1. Main `CMakeLists.txt` File Requirements and Assumptions
Each SegySdk component, including the 'front man', groups own files under a separate folder and has own CMakeLists.txt file.

The main `CMakeLists.txt` file has to satisfy the listed below requirements and assumptions.

The main `CMakeLists.txt` file assumptions:
- all include directories are declared using `PRIVATE` modifier; this is important for the both of own/local and third party headers, otherwise the directories' information might be implicitly shared causing side effects
- all files are listed explicitly, including header and implementation files
- the `segysdk` dependencies are listed in the way to simplify for the linker its jobs, so the top most listed components depend on (some or all the rest) components being listed lower

The main `CMakeLists.txt` file requirements:
- the main library is marked as `SHARED` in contrast to all its components that are linked as a `STATIC` library
- `SEGYSDK_BUILDING_DLL` compile definition is declared using `PRIVATE`, otherwise the definition might be implicitly shared causing side effects
- all the linkage dependencies are listed using `PRIVATE` according to the linker expectations; otherwise the linkage information might be implicitly shared causing side effects

Below is shown a typical `segysdk` main `CMakeLists.txt` file:
```
# Typical CMakeLists.txt file snippent for the main module

add_library(segysdk SHARED 
	# list main component's files here
)

target_include_directories(segysdk
PRIVATE
	# list include directories here including third party ones
)

target_compile_definitions(segysdk
PRIVATE
	SEGYSDK_BUILDING_DLL 
)

target_link_libraries(segysdk
PRIVATE
	# list all the components' STATIC libraries here

PRIVATE
	# list all the third party frameworks and libraries here
)

install(
TARGETS
	segysdk

	# list all the components' STATIC libraries, using the same order as above, here

	EXPORT segysdkTargets
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
	INCLUDES DESTINATION include
)

# add below all the required CMake directives
```
##### 2.2.2.2. How to Deal with "missing reference" to `__declspec(dllimport)` or `__attribute__((dllimport))` Entity Linker Errors
Let us assume some entity has been introduced according to the steps listed in "How to Export Entity" section.

Although the requirements has been satisfied , the linker still shows linkage error for the entity mentioned above.

For most of the cases, the reason for such an error is the linker optimization when all the unreferenced entities are removed from the output binaries.

For now, in the SegySdk is used the following 'fast and dirty' solution: the `segysdk/src/segysdk_public/segy_c_api.h` header file declares the special `segy_dummy_reference_keeping_function` function which body has some simple statements that make some references to the mentioned above entity.

Please refer to the `segy_dummy_reference_keeping_function` function code to get more details how the references have been done for a number of entities.
### 2.4. Third Party Frameworks and Libraries
The present SegySdk depends on the following third party frameworks and libraries:
- SDAPI
- OpenZGY
- JsonCpp
- CppRestSdk
- Thread
- Rpcrt4.lib
- uuid
