project(ToolLogger)

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        InitializeLogger.cpp
    HEADER_FILES
        InitializeLogger.h
    SYSTEM_INCLUDE_DIRECTORIES
        ${jsoncpp_INCLUDE_PATH}
)
