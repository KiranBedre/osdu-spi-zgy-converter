set(DynamicLibraryLoaderSources DynamicLibraryLoader.h DynamicLibraryLoader.cpp)
if(WIN32)
    set(DynamicLibraryLoaderSources ${DynamicLibraryLoaderSources} DynamicLibraryLoaderWin.cpp)
else()
    set(DynamicLibraryLoaderSources ${DynamicLibraryLoaderSources} DynamicLibraryLoaderUnix.cpp)
endif(WIN32)

add_library(segysdk.library_loader STATIC
    ${DynamicLibraryLoaderSources}
    PluginConfig.h PluginConfig.cpp
 )

target_include_directories(segysdk.library_loader
PRIVATE
    ${CMAKE_SEGYSDK_SOURCE_PATH}
    ${CMAKE_SEGYSDK_SOURCE_PATH}/include
    ${jsoncpp_INCLUDE_PATH}
)

target_compile_definitions(segysdk.library_loader 
PRIVATE
    SEGYSDK_BUILDING_DLL
)

if(UNIX)
target_link_libraries(segysdk.library_loader PUBLIC
    ${CMAKE_DL_LIBS}
)
endif(UNIX)
