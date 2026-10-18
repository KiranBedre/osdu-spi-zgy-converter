project(segysdk.telemetry)

include(import_opentelemetry-cpp)
if(${opentelemetry-cpp_FOUND})
    set(CONNECTOR_IMPL ConnectorLive.cpp)
else()
    set(CONNECTOR_IMPL ConnectorStub.cpp)
endif()

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        ${CONNECTOR_IMPL}
        Connector.cpp
    HEADER_FILES
        Connector.h
    LINK_LIBRARIES
        ${OPENTELEMETRY_CPP_LIBRARIES}
)
