
#ifndef FILE_MONITOR_STRING_UTILS_H
#define FILE_MONITOR_STRING_UTILS_H

#include <cstdarg>
#include <string>

namespace fm {
    namespace string_utils {
        std::string string_from_format(const char* format, ...);
        std::string vstring_from_format(const char* format, va_list args);
    }
}

#endif //FILE_MONITOR_STRING_UTILS_H
