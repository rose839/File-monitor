#include <cstdarg>
#include <cstdio>
#include <vector>
#include <string>
#include "string_utils.h"

using std::string;
using std::vector;

namespace fm {
    namespace string_utils {
        std::string vstring_from_format(const char *format, va_list args) {
            size_t current_buffer_size = 0;
            int required_chars = 512;

            vector<char> buffer;

            do {
                current_buffer_size += required_chars;
                buffer.resize(current_buffer_size);
                required_chars = vsnprintf(&buffer[0], current_buffer_size, format, args);
                if (required_chars < 0) {
                    buffer.resize(1);
                    break;
                }
            } while ((size_t)required_chars > current_buffer_size);

            return string(&buffer[0]);
        }

        std::string string_from_format(const char *format, ...) {
            va_list args;
            va_start(args, format);

            string ret = vstring_from_format(format, args);

            va_end(args);
            return ret;
        }
    }
}
