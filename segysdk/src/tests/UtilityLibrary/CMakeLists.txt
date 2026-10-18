add_library(segysdk.testing STATIC
    AutoCleanupTestFile.h AutoCleanupTestFile.cpp
    TestBackdoor.h TestBackdoor.cpp
    TmpCleanupFile.h TmpCleanupFile.cpp
)

target_include_directories(segysdk.testing 
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)
