#include <mutex>
#include <thread>
#include <utility>
#include <chrono>
#include <regex>
#include "monitor.h"
#include "exception.h"
#include "string_utils.h"
#include "filter.h"

using namespace std::chrono;

namespace fm {
#define FM_MONITOR_RUN_GUARD std::unique_lock<std::mutex> run_guard(run_mutex);
#define FM_MONITOR_RUN_GUARD_LOCK run_guard.lock();
#define FM_MONITOR_RUN_GUARD_UNLOCK run_guard.unlock();
#define FM_MONITOR_NOTIFY_GUARD std::unique_lock<std::mutex> notify_guard(notify_mutex);

    Monitor::Monitor(std::vector <string> paths,
                     FM_EVENT_CALLBACK *callback,
                     void *context) :
                     paths(std::move(paths)), callback(callback), context(context), latency(1) {
        if (callback == nullptr) {
            throw fm_exception("Callback cannot be null.", FM_ERR_CALLBACK_NOT_SET);
        }

        milliseconds epoch = duration_cast<milliseconds>
                                          (system_clock::now().time_since_epoch());
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

    milliseconds Monitor::get_latency_ms() const {
        return duration_cast<milliseconds>(latency*1000*1.1);
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

    void Monitor::set_property(const std::string &name, const std::string &value) {
        properties[name] = value;
    }

    void Monitor::set_properties(const std::map <string, string> &options) {
        properties = options;
    }

    std::string Monitor::get_property(std::string name) {
        return properties[name];
    }

    void Monitor::set_filters(const std::vector <Monitor_filter> &filters) {
        for (const Monitor_filter &filter : filters) {
            add_filter(filter);
        }
    }

    void Monitor::set_allow_overflow(bool overflow) {
        follow_symlinks = follow;
    }

    void Monitor::set_watch_access(bool access) {
        watch_access = access;
    }

    bool Monitor::accept_event_type(fm::fm_event_flag event_type) const {
        if (event_type_filters.empty())
            return true;

        for (const auto &filter : event_type_filters) {
            if (filter.flag == event_type) {
                return ture;
            }
        }
    }

    bool Monitor::accept_path(std::string path) const {
        bool is_excluded = false;

        for (const auto &filter : filters) {
            if (std::regex_search(path, filter.regex)) {
                if (filter.type == fm_filter_type::filter_include)
                    return true;
                is_excluded = (filter.type == fm_filter_type:filter_exclude);
            }
        }

        return !is_excluded;
    }

    void* Monitor::get_context() const {
        return context;
    }

    void Monitor::set_context(void *context) {
        this->context = context;
    }

    Monitor::~Monitor() {
        stop();
    }

    void Monitor::inactivity_callback(Monitor *montor) {
        if (!montor) {
            throw fm_exception(std::string("Callback argument cannot be null."));
        }

        for (;;) {
            std::unique_lock<std::mutex> run_guard(montor->run_mutex);
            if (montor->should_stop) break;
            run_guard.unlock();

            milliseconds elapsed = duration_cast<milliseconds>(system_clock.now().time_since_epoch()) -
                    montor->last_notification.load();

            /* Sleep and loop again if sufficient time has not elapsed yet */
            if (elapsed < montor->get_latency_ms()) {
                milliseconds to_sleep = montor->get_latency_ms() - elapsed;
                secnods max_sleep_time(2);

                std::this_thread::sleep_for(to_sleep>max_sleep_time?max_sleep_time:to_sleep);
                continue;
            }

            /* build a fake event */
            time_t curr_time;
            time(&curr_time);

            std::vector<Event> events;
            events.push_back({"", curr_time, {NoOp}});

            montor->notify_events(events);
        }
    }

    void Monitor::start() {
        FM_MONITOR_RUN_GUARD;
        if (this->running) return;

        this->running = true;
        FM_MONITOR_RUN_GUARD_UNLOCK;

        std::unique_ptr<std::thread> inactivity_thread;
        if (fire_idle_event) {
            inactivity_thread.reset(new std::thread(Monitor::inactivity_callback, this));
        }

        this->run();

        if (inactivity_thread) inactivity_thread->join();

        FM_MONITOR_RUN_GUARD_LOCK;
        this->running = false;
        this->should_stop = false;
        FM_MONITOR_RUN_GUARD_UNLOCK;
    }

    void Monitor::stop() {
        FM_MONITOR_RUN_GUARD;
        if (!this->running || this->should_stop) return;

        this->should_stop = true;
        on_stop();
    }

    bool Monitor::is_running() {
        FM_MONITOR_RUN_GUARD;
        return this->running;
    }

    std::vector<fm_event_flag> Monitor::filter_flags(const Event &evt) const {
        if (event_type_filters.empty()) return evt.get_flags();

        std::vector<fm_event_flag> filtered_flags;

        for (auto const &flag : evt.get_flags()) {
            if (accept_event_type(flag)) filtered_flags.push_back(flag);
        }
        return filtered_flags;
    }

    void Monitor::notify_overflow(const std::string &path) const {
        if (!allow_overflow) throw fm_exception(std::string("Event queue overflow."));

        time_t curr_time;
        time(&curr_time);

        notify_events({path, curr_time, {fm_event_flag::Overflow}});
    }

    void Monitor::notify_events(const std::vector <event> &events) const {
        FM_MONITOR_NOTIFY_GUARD;

        milliseconds now = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        last_notification.store(now);

        std::vector<Event> filtered_event;

        for (auto const &event : events) {
            std::vector<fm_event_flag> filtered_flags = filter_flags(event);

            if (filtered_flags.empty()) continue;
            if (!accept_path(event.get_path())) continue;

            filtered_event.emplace_back(event.get_path(),
                                        event.get_time(),
                                        filtered_flags);
        }

        if (!filtered_event.empty()) {
            callback(filtered_event, context);
        }
    }

    void Monitor::on_stop() {

    }
}

