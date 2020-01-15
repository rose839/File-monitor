option(PACKAGE_NAME "package name" ON)

set(SOURCE_FILES fmonitor.cpp
        fmonitor.h
        config.h)
        
add_executable(fmonitor ${SOURCE_FILES}) 
target_include_directories(fmonitor PUBLIC ../libfile_monitor/src .) 
target_link_libraries(fmonitor LINK_PUBLIC libfile_monitor.so)
