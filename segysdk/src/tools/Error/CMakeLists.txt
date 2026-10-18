project(ToolError)

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        Error.cpp
    HEADER_FILES
        Error.h
)
