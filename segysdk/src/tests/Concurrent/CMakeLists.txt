find_package (Threads)

add_executable(UnitTests.BufferQueue
    TestBufferQueue.cpp
)

target_include_directories(UnitTests.BufferQueue
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
)

target_link_libraries(UnitTests.BufferQueue
PRIVATE
    segysdk
    gtest_main
    ${CMAKE_THREAD_LIBS_INIT}
)

add_executable(UnitTests.ThreadPool
    TestThreadPool.cpp
)

target_include_directories(UnitTests.ThreadPool
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
)

target_link_libraries(UnitTests.ThreadPool
PRIVATE
    segysdk
    gtest_main
    ${CMAKE_THREAD_LIBS_INIT}
)


include(GoogleTest)
gtest_discover_tests(UnitTests.BufferQueue)
gtest_discover_tests(UnitTests.ThreadPool)

add_test(
    NAME UnitTests.BufferQueue 
    COMMAND UnitTests.BufferQueue
)

add_test(
    NAME UnitTests.ThreadPool
    COMMAND UnitTests.ThreadPool
)