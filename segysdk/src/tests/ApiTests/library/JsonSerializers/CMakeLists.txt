add_library(JsonSerializers STATIC
    JsonSerializer.h 
    IndexFieldJson.h IndexFieldJson.cpp
    HeaderDataStructJson.h HeaderDataStructJson.cpp
    TraceJson.h TraceJson.cpp
    JsonDeserializer.h
    JsonException.h
)

target_include_directories(JsonSerializers
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_link_libraries(JsonSerializers
PRIVATE
    segysdk
    gtest_main
    ${JSON_LINK_LIBS}
)
