project(UnitTests.Request)

include(AddUnitTest)

AddUnitTest(${PROJECT_NAME}
    SOURCE_FILES
        ${CMAKE_SEGYSDK_SOURCE_PATH}/Request/Params.cpp
        ParamsTest.cpp
)
