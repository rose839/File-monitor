#include <mutex>
#include <thread>
#include <utility>
#include <chrono>
#include <regex>
#include "monitor.h"
#include "exception.h"
#include "string_utils.h"

namespace fm {
#define FM_MONITOR_RUN_GUARD std::unique_lock<std::mutex> run_guard(run_mutex);
#define FM_MONITOR_RUN_GUARD_LOCK run_guard.lock();
#define FM_MONITOR_RUN_GUARD_UNLOCK run_guard.unlock();

    Monitor::Monitor(std::vector <string> paths,
                     FM_EVENT_CALLBACK *callback,
                     void *context) :
                     paths(std::move(paths)), callback(callback), context(context), latency(1) {
        if (callback == nullptr) {
            throw fm_exception("Callback cannot be null.", FM_ERR_CALLBACK_NOT_SET);
        }

        std::chrono::milliseconds epoch = std::chrono::duration_cast<std::chrono::milliseconds>
                                          (std::chrono::system_clock::now().time_since_epoch());
        last_notification.store(epoch);
    }

    void Monitor::set_allow_overflow(bool overflow) {
        allow_overflow = overflow;
    }

    void Monitor::set_latency(double latency) {
        if (latency < 0) {
            throw fm_exception("Latency cannot be negative.", FSW_ERR_INVALID_LATENCY);
        }
        this->latency = latency;
    }

    void Monitor::set_fire_idle_event(bool fire_idle_event) {
        this->fire_idle_event = fire_idle_event;
    }

    std::chrono::milliseconds Monitor::get_latency_ms() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(latency*1000*1.1);
    }

    void Monitor::set_recursive(bool recursive) {
        this->recursive = recursive;
    }

    void monitor::set_directory_only(bool directory_only) {
        this->directory_only = directory_only;
    }

    void Monitor::add_event_type_filter(const EVENT_TYPE_FILTER &filter) {
        this->event_type_filters.push_back(filter);
    }

    void Monitor::set_event_type_filters(const std::vector<EVENT_TYPE_FILTER> &filters) {
        event_type_filters.clear();
        for (const auto &filter : filters) {
            add_event_type_filter(filter);
        }
    }

    void Monitor::add_filter(const Monitor_filter &filter) {
        std::regex::flag_type regex_flags = std::regex::basic;

        if (filter.extended) {
            regex_flags = std::regex::extended;
        }
        if (!filter.case_sensitive) {
            regex_flags |= std::regex::icase;
        }

        try {
            this->filters.push_back({std::regex(filter.text, regex_flags), filter.type});
        } catch (std::regex_error &error) {
            throw fm_exception(string_utils::string_from_format("An error occurred during the compilation of %s",
                                                                filter.text.c_str(),
                                                                FM_ERR_INVALID_REGEX));
        }
    }
}

