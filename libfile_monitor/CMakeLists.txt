include_directories(.. src)

add_definitions(-DHAVE_CONFIG_H)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

set(LIB_SOURCE_FILES
        src/error.h
        src/event.cpp
        src/event.h
        src/exception.cpp
        src/exception.h
        src/filter.cpp
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
        src/path_utils.cpp
        src/path_utils.h)

include(CheckIncludeFiles)
CHECK_INCLUDE_FILES(sys/inotify.h HAVE_SYS_INOTIFY_H)
if (HAVE_SYS_INOTIFY_H)
    set(LIB_SOURCE_FILES
            ${LIB_SOURCE_FILES}
            src/inotify_monitor.cpp
            src/inotify_monitor.h)
endif (HAVE_SYS_INOTIFY_H)

add_library(file_monitor SHARED ${LIB_SOURCE_FILES})
target_include_directories(file_monitor PUBLIC src ${PROJECT_BINARY_DIR}/include)
