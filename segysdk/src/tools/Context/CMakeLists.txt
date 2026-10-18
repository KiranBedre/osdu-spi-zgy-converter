project(ToolContext)

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        Context.cpp
    HEADER_FILES
        Context.h
    LINK_LIBRARIES
        ${JSON_LINK_LIBS}
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
)
