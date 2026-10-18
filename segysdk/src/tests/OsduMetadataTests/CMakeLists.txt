add_executable(UnitTests.OsduMetadataUtilities
    MetadataBaseTests.cpp
    MetadataHelperTests.cpp
    OsduAbstractCommonResourcesMetadataTests.cpp
    OsduAbstractDatasetTests.cpp
    OsduAbstractFileCollectionTests.cpp
    OsduAbstractFileSourceInfoTests.cpp
    OsduAclMetadataTests.cpp
    OsduAncestryMetadataTests.cpp
    OsduCommonMetadataTests.cpp
    OsduFileCollectionSegyTests.cpp
    OsduFileSeismicHistogramTests.cpp
    OsduFileSeismicLineGeometryTests.cpp
    OsduLegalMetadataTests.cpp
    OsduOverridesTests.cpp
    OsduMetadataTestBase.h
    OsduSeismic2DInterpretationSetTests.cpp
    OsduSeismicBinGridTests.cpp
    OsduSeismicHistogramTests.cpp
    OsduSeismicLineGeometryTests.cpp
    OsduSeismicTraceDataTests.cpp
    OsduSeismicTraceDataFullyPopulatedTest.cpp
    OsduSeismicTraceNavigationTests.cpp
    OsduVectorHeaderMappingTests.cpp
    OsduWorkProductTests.cpp
    OsduFileCollectionOpenZgyTests.cpp
)


target_include_directories(UnitTests.OsduMetadataUtilities
PRIVATE	
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
    ${vcpkg_INCLUDE_PATH}
)

target_link_libraries(UnitTests.OsduMetadataUtilities
PRIVATE
    segysdk
    gtest_main
    ${JSON_LINK_LIBS}
    ${CPPRESTSDK_LINK_LIBS}
)

include(GoogleTest)
gtest_discover_tests(UnitTests.OsduMetadataUtilities)
add_test(
    NAME UnitTests.OsduMetadataUtilities
    COMMAND UnitTests.OsduMetadataUtilities
)