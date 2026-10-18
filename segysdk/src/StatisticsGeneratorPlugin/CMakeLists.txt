include(import_cpprestsdk)

add_library(segysdk.statistics_generator_plugin STATIC
    StatisticsInputParams.h   StatisticsInputParams.cpp
    StatisticsGeneratorInterface.h
    StatisticsGeneratorPluginException.h
    StatisticsGeneratorPlugin.h StatisticsGeneratorPlugin.cpp
    StatisticsGeneratorCInterface.h
)

target_include_directories(segysdk.statistics_generator_plugin 
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
	${jsoncpp_INCLUDE_PATH}
)

target_compile_definitions(segysdk.statistics_generator_plugin 
PRIVATE
    SEGYSDK_BUILDING_DLL
)
