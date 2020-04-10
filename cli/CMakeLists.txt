option(PACKAGE_NAME "package name" ON)
option(VERSION_STRING "version string" ON)

set(SOURCE_FILES fmonitor.cpp
        fmonitor.h)
        
add_executable(fmonitor ${SOURCE_FILES}) 
target_include_directories(fmonitor PUBLIC ../libfile_monitor/src . ${PROJECT_BINARY_DIR}/include) 
target_link_libraries(fmonitor LINK_PUBLIC ${CMAKE_BINARY_DIR}/lib/libfile_monitor.so)
