add_library(segysdk.logger STATIC
    Message.h Message.cpp
    MessageGSM.h MessageGSM.cpp
    MessagePool.h MessagePool.cpp
    LogWriterBase.h LogWriterBase.cpp
    LogWriterDispatcher.h LogWriterDispatcher.cpp
    LogWriterOstream.h LogWriterOstream.cpp
    LoggerSettings.h LoggerSettings.cpp
    Logger.h Logger.cpp
    ProgressReporting.h ProgressReporting.cpp
)

target_include_directories(segysdk.logger
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_compile_definitions(segysdk.logger 
PRIVATE
    SEGYSDK_BUILDING_DLL
)

add_subdirectory(Gsm)
