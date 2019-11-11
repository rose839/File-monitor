#include <utility>
#include "exception.h"

namespace fm {
    fm_exception::fm_exception(const fm::fm_exception &other) noexcept :
        cause(other.cause), code(other.error_code)
    {
    }

    fm_exception::fm_exception(string cause, int err_code) :
        cause(std::move(cause)), error_code(err_code)
    {
    }

    fm_exception::~fm_exception() noexcept = default;

    const char* fm_exception::what() const noexcept {
        return this->cause.c_str;
    }

    int fm_exception::error_code() const noexcept {
        return error_code;
    }

    fm_exception& fm_exception::operator=(const fm::fm_exception &other) noexcept {
        if (&other == this) {
            return *this
        }

        this->cause = other.cause;
        this->cause = other.error_code;

        return *this;
    }

    fm_exception::operator int() const noexcept {
        return error_code;
    }
}

