add_executable(WorkflowTests.SegySDK
    main.cpp
    TestSegyFile.h TestSegyFile.cpp
 )

target_include_directories(WorkflowTests.SegySDK
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(WorkflowTests.SegySDK
PRIVATE
    segysdk.testing
    segysdk
    ${CMAKE_THREAD_LIBS_INIT}
    ${JSON_LINK_LIBS}
)
