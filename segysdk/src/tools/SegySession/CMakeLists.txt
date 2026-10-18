project(ToolSegySession)

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        CreateSession.cpp
    HEADER_FILES
        CreateSession.h
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
    LINK_LIBRARIES
        ${JSON_LINK_LIBS}
        segysdk
        ToolError
        ToolParams
)
