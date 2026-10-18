include(GoogleTest)

add_executable(UnitTests.DynamicLibraryLoader
    DynamicLibraryLoaderTests.cpp
)
target_include_directories(UnitTests.DynamicLibraryLoader
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
)
target_link_libraries(UnitTests.DynamicLibraryLoader
PRIVATE
	segysdk
    gtest_main
)
gtest_discover_tests(UnitTests.DynamicLibraryLoader)

# This to be used in DynamicLibraryLoaderTests
add_library(DummyDLL SHARED
    DummyDLL.h
    DummyDLL.cpp
)
target_compile_definitions(DummyDLL
PUBLIC
    SEGYSDK_BUILDING_DLL 
)
target_include_directories(DummyDLL
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
)

add_test(
    NAME DynamicLibraryLoaderTests
    COMMAND UnitTests.DynamicLibraryLoader
)