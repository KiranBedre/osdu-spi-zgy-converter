add_executable(WorkflowTests.SegyIndex
    Runner.cpp
    SegyIndexWorkflowTest.h SegyIndexWorkflowTest.cpp
    SegyTestVolumeDefinition.h  SegyTestVolumeDefinition.cpp
)

target_include_directories(WorkflowTests.SegyIndex
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(WorkflowTests.SegyIndex
PRIVATE
    segysdk.testing
    segysdk
    ${JSON_LINK_LIBS}
)
