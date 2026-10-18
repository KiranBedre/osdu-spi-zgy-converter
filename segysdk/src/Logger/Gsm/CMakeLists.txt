include(import_cpprestsdk)

add_library(segysdk.logger.gsm SHARED
    LogWriterGSM.h LogWriterGSM.cpp
)

target_compile_definitions(segysdk.logger.gsm
PRIVATE
    SEGYSDK_BUILDING_DLL 
)

target_include_directories(segysdk.logger.gsm
PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(segysdk.logger.gsm 
PRIVATE 
    segysdk
    ${JSON_LINK_LIBS}
	${CPPRESTSDK_LINK_LIBS}
)
