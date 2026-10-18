project(ToolOptions)

find_package(Boost COMPONENTS program_options REQUIRED)

include(AddLibrary)

AddStaticLibrary(${PROJECT_NAME}
    SOURCE_FILES
        Options.cpp
    HEADER_FILES
        Options.h
    LINK_LIBRARIES
        ${Boost_LIBRARIES}
)
