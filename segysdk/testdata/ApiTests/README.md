# SegySDK API Test Framework

The API test framework is designed based on GTest and intends to provide an easy way to implement tests for various SegySDK APIs with a minimal amount of coding required.

The core of the framework is the `ApiTestBase` class. It handles almost all actions required for tests, including reading test configuration from JSON files, instantiating parameterized tests, validating results based on a configuration, etc. Another important part is JSON configuration files. They define different parameters that can be used in tests and provide flexible configurations for expected test results.

## Folder structure

API test source code is in the *./segysdk/src/tests/ApiTests/tests* folder. Each API class has its subfolder and executable. The typical naming convention for those modules is *ApiClassName* with the *“-ApiTests”* suffix. To date, there are `SegySessionFactoryApiTests`, `SegySessionApiTests`, `SegyIndexApiTests`, and `SegyBulkReaderApiTest`.

Test configuration files are stored in *./segysdk/testdata/ApiTests*. Baselines are stored in subfolders of *./segysdk/testdata/ApiTests/baselines/*. Each subfolder corresponds to the API class that is being tested.

Many tests require some input and/or baseline files to be provided. The test framework operates with the assumption, that test data are located in *./segysdk/testdata* folder. All path arguments specified in the JSON configuration file are assumed to be relative to this folder.

## Test creation

To create a new test/test suite, follow the following steps:
1. Create a new *\*.cpp* file under *./segysdk/src/tests/ApiTests/tests/* directory. The filename should be “***ApiClassName**Tests.cpp*”, where **ApiClassName** is the name of the API class.
2. Define a new executable in *./segysdk/src/tests/ApiTests/tests/CMakeLists.txt* using the `AddTest()` CMake function:
```
# ApiClassName
AddTest(ApiClassNameTests
NOT_BASE_GTEST
TEST_SOURCE_FILES
	ApiClassNameTests.cpp
EXTRA_INCLUDE_DIRECTORIES
	${jsoncpp_INCLUDE_PATH}
EXTRA_LINK_LIBRARIES
	ApiTestFramework
	${JSON_LINK_LIBS}
GTEST_OPTIONS
	NO_PRETTY_VALUES
	PROPERTIES LABELS API_TESTS
)
```
3. In a newly created *\*.cpp* file, add the copyright header and include the following framework files:
```
#include "../library/ApiTestFramework/ApiTest.h"
#include "../library/ApiTestFramework/ApiTestBase.h"
```
4. Define namespace:
```
namespace segysdk::api::tests
{
}
```
5. Create a new test class and derive it from `ApiTestBase`:
```
class ApiClassNameMethodName : public ApiTestBase
{
};
```
6. Add `run()` method:
```
public:
	auto run()
	{
	}
```
7. Get the required parameters from a JSON test configuration file, like the following:
```
auto param = getParameter<Type>("paramName");
```
, where `Type` is a required parameter type. For now, POD, `std::string`, `std::vector<POD/std::string>` and `Json::Value` are supported out-of-the-box. For additional data types, the `JsonDeserializer` class can be used.

8. Return API call:
```
return ApiClassInstance.apiMethodName(param);
```
, or some output value from API call:
```
Type retParam;
ApiClassInstance.apiMethodName(param, &retParam);
return retParam;
```
9. Define test after class declaration using `TEST_API()` macro with test class name and a path to JSON configuration file:
```
TEST_API(ApiClassNameMethodName, "ApiTests/ApiClassNameTests.json");
```
10. Add the following code at the end of the file outside of the namespace to enable support for the test framework command line options:
```
int
main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	segysdk::api::tests::ApiEnvironment::instantiateFromCLI(argc, argv);
	return RUN_ALL_TESTS();
}
```

## Test configuration JSON

The test configuration JSON file defines different parameter values for test runs and allows specifying the expected test behavior and results.

The root object of configuration JSON has a single field **“tests”** that contains an array of test definition objects. Each test definition must specify the **“name”** field with the test name that must be the same as the corresponding test class in the source code, and **“data”** – a collection of parameter-result definitions. 

The parameter-result definition object has the following fields:

- **“parameters”** – it is a collection of key-value pairs that define parameters that can be used in the test. The key is a string by which the parameter can be accessed in a test. The value can be of any supported JSON type (boolean, integer, number, string, or JSON itself) or an array of elements of the same type. The exact type of the parameter is defined by a test source code. In case the value specified in the configuration file cannot be converted to a type specified in a test source code, an error will be thrown during a test execution.
- **“result”** – specifies expected test result. Based on its value, additional fields in the definition are required. The **“result”** can be one of the following strings:
	- *“success”* – specifies that a test doesn’t generate any results and passes without an error;
	- *“value”* – no error and a test code should return the value specified by the **“value”** field in the parameter-results definition;
	- *“vector”* – no error and a test code returns a vector of values. Those values will be serialized to a binary file and compared with another file specified in the **“baseline”** field in the definition;
	- *“object”* – it’s expected that the result of test execution is a pointer to an object of some type. The framework simply checks that the returned object is not NULL. Supports raw pointers, `std::unique_ptr<T>` and `std::shared_ptr<T>`;
	- *“file”* – expects that a test doesn’t return anything but generates some file in the file system. The **“file”** field specifies an expected generated file and it’s then compared with another one defined by the **“baseline”** field;
	- *“text”* – expects that a test returns a string. This string is then written to a text file that is then compared with another file defined by the **“textfile”** field;
	- *“json”* – expects that a test returns JSON string. It is then parsed to `Json::Value` and compared with another `Json::Value` object that is read from a file specified by the **“jsonfile”** field;
	- *“json_file”* – similar to the *“file”* result, a test is expected to generate a file, but here it’s a JSON file. The **“file”** field specifies an expected file and  it’s compared to one defined by the **“jsonfile”** field. JSON comparison is applied;
	- *“throws”* – expects that a test execution will result in an exception. An exception message will be compared with a string specified by the **“exception”** field.

> NOTE: The fields that correspond to a file should contain paths relative to *./segysdk/testdata/*.

> NOTE: *“vector”*, *“file”* and *“text”* test results use binary comparison to check the result.

> NOTE: When comparing JSON, special substitutes can be used in baselines:
> - **${DATE_TIME}** value for a JSON field means that it contains a date-time value. As those often depend on when a test is run, they will be ignored during comparison;
> - **${PARENTDIR}**. Some fields in generated JSON can contain a path to a dataset. As paths depend on the environment a test is run on, this substitute will be replaced with a parent directory path of a dataset.

> NOTE: When comparing float-point numbers in JSON, the precision is 1e-9.

The below table summarizes the required fields based on the **“result”** field value:

| “result”    | Required fields | Type          | Description                                                                                                      |
| ------------- | --------------- | ------------- | ---------------------------------------------------------------------------------------------------------------- |
| *“success”*   | None            | N/A           | N/A                                                                                                              |
| *“value”*     | **“value”**     | Boolean, integer, number, string, JSON object, or array of elements of the same type | A value that a test is expected to return |
| *“vector”*    | **“baseline”**  | String (path) | Path to the binary file that contains serialized vector (std::vector<T>::data() is used in serialization)        |
| *“object”*    | None            | N/A           | N/A                                                                                                              |
| *“file”*      | **“file”**      | String (path) | Path to a file that a test should generate                                                                       |
|               | **“baseline”**  | String (path) | Path to a baseline file to compare with                                                                          |
| *“text”*      | **“textfile”**  | String (path) | Path to a text file to compare resulting text with                                                               |
| *“json”*      | **“jsonfile”**  | String (path) | Path to a JSON file to compare resulting JSON with                                                               |
| *“json_file”* | **“file”**      | String (path) | Path to a JSON file a test should generate                                                                       |
|               | **“jsonfile”**  | String (path) | Path to a baseline JSON file to compare results with                                                             |
| *“throws”*    | **“exception”** | String        | Expected exception message                                                                                       |

## Additional capabilities of API Test Framework

In case a test requires some additional initialization/cleanup, it is possible to override `SetUp()/TearDown()` methods that are run respectively before and after the execution of a test itself. Here you can initialize some test class fields (e.g.: `SegySession` class instance), add files to be removed after the test execution, etc.

> NOTE: Call base class `SetUp()/TearDown()` methods at the beginning/end of the overridden method.

> NOTE: As different tests can require the same initialization, an intermediate base class can be created containing the required overrides of the `SetUp()/TearDown()` methods.

Also, the `ApiTestBase` class provides the following utility methods:

- `template<typename T> T getParameter(const std::string& name) const` - returns a named parameter from a configuration file converted to a specified type T. If the parameter is missing in a test configuration, an error occurs and a test fails;
- `template<typename T> std::optional<T> getOptionalParameter(const std::string& name)` – tries to get a parameter from a configuration file. If no parameter is defined, it returns an empty value and no failure occurs;
- `std::filesystem::path getTestdataPathParameter(const std::string& name)` – reads a path parameter from a configuration file and appends it to *./segysdk/testdata/* folder on the run environment. A parameter must be of a string type;
- `std::filesystem::path getCurrentPathParameter(const std::string& name)` – similar to above, but appends a path to the current working directory.
- `void addToCleanup(std::filesystem::path filePath)` – adds a file to the list to be removed after the test execution.

Additionally, the framework provides the helper utilities to serialize/deserialize JSON. There are two main helper classes: `JsonSerializer` and `JsonDeserializer`.
The `JsonSerializer` class provides several static overloaded `toJson()` methods to convert typed values to JSON. There are:
- `template <typename Serializer, typename T> static Json::Value toJson(const std::string& memberName, const T& value)` - serializes value using Serializer and returns it as `{ "memberName" : serializarion_results }`. The value here can be a vector.
- `template <typename T> static Json::Value toJson(const std::string& memberName, const T& value)` - converts POD, `std::string`, or `std::vector` of the former to JSON and returns it as `{ "memberName" : serialization_results }`.
- `template <typename Serializer, typename T> static Json::Value toJson(const T& value)` - returns JSON object generated by `Serializer` class. If a value type is `std::vector`, returns JSON array where each element is a result of calling `Serializer::toJson()` on an element of `std::vector`.

Other overloads of `toJson()` methods implement the required functionality to allow serializing of PODs and `std::vector`-s. However, they do not produce a JSON object that can be written to a file and should not be used in a test.

The `Serializer` type in the methods above should implement the `static Json::Value toJson(const Type& value)` method, where `Type` is a specific struct/class from SegySDK. A generated JSON object should be writable to a file and usually consists of key-value pairs that correspond to `Type` fields.

The `JsonDeserializer` class provides two methods to deserialize values from JSON:
- `template<typename T> static T fromJson(const Json::Value& json)` - deserializes POD, `std::string`, or `std::vector` of the former from JSON object. In case JSON cannot be converted to a desired type, an exception is thrown.
- `template<typename Deserializer, typename T> static T fromJson(const Json::Value& json)` - tries to create an object of type `T` or an `std::vector` of some other type from JSON using the `Deserializer` type.

The `Deserializer` type should provide the `static T fromJson(const Json::Value&)` method, where type `T` is some SegySDK struct.

## API Test Framework command line options

To date, the framework provides two command line options:

- `-keepOutputFiles` – turns off removal of files that are generated by a test run. It includes files added using the `addToCleanup()` method, as well as files specified by the **“-file”** fields from a configuration file;
- `-dumpBaselines` – creates a dump file with a test result for the following **“result”** field values: *“vector”*, *“file”*, *“text”*, *“json”* and *“json_file”*. The file is created alongside a baseline, with the same filename but with *-.dmp-* suffix

Also, it is convenient to filter tests to be run with the GTest [*–gtest_filter*](https://google.github.io/googletest/advanced.html#running-a-subset-of-the-tests) option.
