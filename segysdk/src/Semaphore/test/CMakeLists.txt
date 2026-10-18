project(UnitTests.Semaphore)

include(AddUnitTest)

AddUnitTest(${PROJECT_NAME}
    SOURCE_FILES
        ${CMAKE_SEGYSDK_SOURCE_PATH}/Semaphore/Semaphore.cpp
        SemaphoreTest.cpp
)
