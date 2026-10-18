include(CheckCXXSymbolExists)

check_cxx_symbol_exists(gmtime_r "ctime" HAVE_GMTIME_R)
try_compile(HAVE_MSVC_GMTIME_S ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_LIST_DIR}/check_msvc_gmtime_s.cpp)
