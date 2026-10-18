
include(import_cpprestsdk)

add_library(segysdk.utilities STATIC
    Adapters.h Adapters.cpp
    Authorization.h Authorization.cpp
    BinGrid.h   BinGrid.cpp
    BrickSize.h   BrickSize.cpp
    Containers.h
    CrsConverterSvcClient.h		CrsConverterSvcClient.cpp

    Ensure.h
    EnsureWrapper.h
    Environment.h  Environment.cpp
    EnvironmentVariables.h  EnvironmentVariables.cpp
    EnvironmentVariableStringConsts.h
    GuidGenerator.h   GuidGenerator.cpp
    InputParser.h  InputParser.cpp
    JsonBinGrid.h  JsonBinGrid.cpp
    JsonUtilities.h  JsonUtilities.cpp
    JsonException.h
    MeasureDuration.h
    TokenSourceFactory.h  TokenSourceFactory.cpp
    OpenFileProperties.h  OpenFileProperties.cpp
    Options.h   Options.cpp
    OptionsDelfiCommandLine.h  OptionsDelfiCommandLine.cpp
    OptionsOsdu.h  OptionsOsdu.cpp
    PluginUtilities.h     PluginUtilities.cpp
    ReadTextFile.h  ReadTextFile.cpp
    SafeTime.h SafeTime.cpp
    SessionUtilities.h   SessionUtilities.cpp
    StringUtils.h  StringUtils.cpp
    StorageSvcClient.h  StorageSvcClient.cpp
    Settings.h Settings.cpp
    ZgyOutputParams.h  ZgyOutputParams.cpp

    auth/BearerTokenAuth.h  auth/BearerTokenAuth.cpp
    auth/CachingTokenSource.h  auth/CachingTokenSource.cpp
    auth/FailingTokenSource.h auth/FailingTokenSource.cpp
    auth/HttpCredentialProvider.h  auth/HttpCredentialProvider.cpp
    auth/Jwt.h  auth/Jwt.cpp
    auth/TokenSource.h auth/TokenSource.cpp
    auth/StaticTokenSource.h auth/StaticTokenSource.cpp

    delfi/DelfiBinGrid.h  delfi/DelfiBinGrid.cpp

    osdu/GeneratedOutputFileName.h  osdu/GeneratedOutputFileName.cpp
    osdu/OsduFieldNameLiterals.h  osdu/OsduFieldNameLiterals.cpp
    osdu/OsduFileCollectionSegy100.h  osdu/OsduFileCollectionSegy100.cpp
    osdu/OsduKind.h  osdu/OsduKind.cpp
    osdu/OsduObject.h  osdu/OsduObject.cpp
    osdu/OsduObjectReference.h  osdu/OsduObjectReference.cpp
    osdu/OsduSeismicBinGrid100.h  osdu/OsduSeismicBinGrid100.cpp
    osdu/OsduSeismicTraceData100.h  osdu/OsduSeismicTraceData100.cpp
    osdu/OsduSeismicTraceData140.h  osdu/OsduSeismicTraceData140.cpp
    osdu/OsduSeismicTraceDataFactory.h  osdu/OsduSeismicTraceDataFactory.cpp
    osdu/OsduWorkProduct100.h  osdu/OsduWorkProduct100.cpp
    osdu/WellKnownTypes.h

)

target_include_directories(segysdk.utilities
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${OPENZGY_INCLUDE_DIR}
    ${jsoncpp_INCLUDE_PATH}
    ${vcpkg_INCLUDE_PATH}
)

target_compile_definitions(segysdk.utilities
PRIVATE
    SEGYSDK_BUILDING_DLL
)

target_link_libraries(segysdk.utilities
PRIVATE
    ${CPPRESTSDK_LINK_LIBS}
)

include(AddLibrary)

AddStaticLibrary(segysdk.json_parser
    SOURCE_FILES
        JsonParser.cpp
    HEADER_FILES
        JsonParser.h
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
    LINK_LIBRARIES
        ${JSON_LINK_LIBS}
)
