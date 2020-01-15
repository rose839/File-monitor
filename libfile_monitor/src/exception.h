
#ifndef FILE_MONITOR_EXCEPTION_H
#define FILE_MONITOR_EXCEPTION_H

#include <exception.h>
#include <string>
#include "error.h"

using std::string;

namespace fm {
    class fm_exception : public std::exception {
    public:
        fm_exception(string cause, int err_code = FM_ERR_UNKNOWN_ERROR);
        fm_exception(const fm_exception &other) noexcept;
        fm_exception& operator=(const fm_exception &other) noexcept;

        virtual const char* what() const noexcept;
        virtual int err_code() const noexcept;
        virtual ~fm_exception() noexcept;

        /*
         * Get error code.
         * */
        explicit operator int () const noexcept;

    private:
        string cause;
        int error_code;
    };
}

#endif //FILE_MONITOR_EXCEPTION_H
