include_directories(.. src)

add_definitions(-DHAVE_CONFIG_H)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config.h.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/include/config.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

set(LIB_SOURCE_FILES
        src/error.h
        src/event.cpp
        src/event.h
        src/exception.cpp
        src/exception.h
        src/filter.h
        src/log.cpp
        src/log.h
        src/monitor.cpp
        src/monitor.h
        src/monitor_factory.cpp
        src/monitor_factory.h
        src/poll_monitor.cpp
        src/poll_monitor.h
        src/string_utils.cpp
        src/string_utils.h
        config.h)

include(CheckIncludeFiles)
CHECK_INCLUDE_FILES(sys/inotify.h HAVE_SYS_INOTIFY_H)
if (HAVE_SYS_INOTIFY_H)
    set(LIB_SOURCE_FILES
            ${LIB_SOURCE_FILES}
            src/inotify_monitor.cpp
            src/inotify_monitor.h)
endif (HAVE_SYS_INOTIFY_H)

add_library(libfilemonitor ${LIB_SOURCE_FILES})
target_include_directories(libfilemonitor PUBLIC src)