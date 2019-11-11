/*
 * @brief Header of the fm::monitor class.
 *
 * This header file defines the fsw::monitor class, the base type of a
 * `libfile_monitor` monitor.
 * */

#ifndef FILE_MONITOR_MONITOR_H
#define FILE_MONITOR_MONITOR_H

#include <string>
#include <vector>
#include "event.h"

namespace fm{

    typedef struct _compiled_monitor_filter {
        std::regex regex;
        fsw_filter_type type;
    }COMPILED_MONITOR_FILTER_S;

    /*
     * This enumeration lists all the available monitors, you can
     * add the platform-specific default monitor.
     * */
    enum fm_monitor_type
    {
        system_default_monitor_type = 0, /*System default monitor. */
        inotify_monitor_type,            /*Linux `inotify` monitor. */
        poll_monitor_type,               /* `stat()`-based poll monitor. */
    };

    /*
     * @brief Function definition of an event callback.
     * The event callback is a user-supplied function that is invoked by the
     * monitor when an event is detected. The following parameters are passed to
     * the callback:
     *
     *    - A reference to the vector of events.
     *    - A pointer to the context data set by the caller.
     * */
    typedef void FM_EVENT_CALLBACK(const std::vector<event>&, void *);

    /*
     * @brief Base class of all monitors.
     *
     * This class encapsulates the common functionality of a monitor.
     *
     *    - Accessors to configuration parameters.
     *    - start() and stop() lifecycle.
     *    - Event filtering.
     *    - Event notification to user-provided callback function.
     *
     * At least the following tasks must be performed to implement a monitor:
     *
     *    - Providing an implementation of the run() method.
     *    - Providing an implementation of the on_stop() method if the
     *      monitor cannot be stopped cooperatively from the run() method.
     *
     * A basic monitor needs to implement the run() method, whose skeleton is
     * often similar to the following:
     *
     *     void run()
     *     {
     *       initialize_api();
     *
     *       for (;;)
     *       {
     *           unique_lock<mutex> run_guard(run_mutex);
     *           if (should_stop) break;
     *           run_guard.unlock();
     *
     *         scan_paths();
     *         wait_for_events();
     *
     *         vector<change_events> evts = get_changes();
     *         vector<event> events;
     *
     *         for (auto & evt : evts)
     *         {
     *           if (accept(evt.get_path))
     *           {
     *             events.push_back({event from evt});
     *           }
     *         }
     *
     *         if (events.size()) notify_events(events);
     *       }
     *
     *       terminate_api();
     *     }
     *
     * Despite being a minimal implementation, it performs all the tasks commonly
     * performed by a monitor:
     *
     *   - It initializes the API it uses to detect file change events.
     *
     *   - It enters a loop, often infinite, where change events are waited for.
     *
     *   - Tt locks on monitor::run_mutex to check whether monitor::should_stop
     *     is set to true.  If it is, the monitor breaks the loop to return from
     *     run() as soon as possible.
     *
     *   - It scans the paths that must be observed: this step might be necessary
     *     for example because some path may not have existed during the previous
     *     iteration of the loop, or because some API may require the user to
     *     re-register a watch on a path after events are retrieved.
     *
     *   - The notify_events() method is called to filter the event types and
     *     notify the caller.
     * */
    class Monitor {
    public:
        Monitor(std::vector<std::string> paths,
                FM_EVENT_CALLBACK *callback,
                void *context = nullptr);
        virtual ~Monitor();
        Monitor(const Moniter &orig) = delete;
        Monitor &operator=(const Monitor &that) = delete;

        void set_property(const std::string &name, const std::string &value);
        void set_properties(const std::map<std::string, std::string> &options);
        std::string get_property(std::string name);

        /*
         * The latency is a positive number that indicates to a monitor
         * implementation how often events must be retrieved or waited
         * for: the shortest the latency, the quicker events are processed.
         * */
        void set_latency(double latency);

        /*
         * the fire_idle_event flag instructs the monitor to fire a
         * fake event at the event of an _idle_ cycle.  An idle cycle
         * is a period of time whose length is 110% of the monitor::latency
         * where no change events were detected.
         * */
        void set_fire_idle_event(bool fire_idle_event);

        /*
         * If this flag is set, the monitor will report a monitor buffer overflow as
         * a change event of type fm_event_flag::Overflow.
         * */
        void set_allow_overflow(bool overflow);

        void set_recursive(bool recursive);
        void set_directory_only(bool directory_only);
        void set_follow_symlinks(bool follow);

        void *get_context() const;
        void set_context(void *context);

        void add_filter(const Monitor_filter &filter);
        void set_filters(const std::vector<Monitor_filter> &filters);

        void add_event_type_filter(const EVENT_TYPE_FILTER &filter);
        void set_event_type_filters(
                const std::vector<fsw_event_type_filter>& filters);

        /*
         * The monitor status is marked running and it starts watching for
         * change events. This function performs the following tasks:
         *
         *    - Atomically marks the thread state as running, locking on
         *      Monitor::run_mutex.
         *    - Calls the run() function: the Monitor::run_mutex is not locked
         *      during this call.
         *    - When run() returns, it atomically marks the thread state as
         *      stopped, locking on Monitor::run_mutex.
         *
         * This call does _not_ return until the monitor is stopped and events are
         * notified from its thread.
         * */
        void start();

        /*
         * stop() is designed to be called from another thread.
         * stop() is a cooperative signal that must be handled in an
         * implementation-specific way in the run() function.
         * */
        void stop();

        bool is_running();

        /*
         * Monitor file access events.
         * */
        void set_watch_access(bool access);

    protected:
        bool accept_event_type(enum fm_event_flag flag) const;
        bool accept_path(std::string path) const;
        void notify_events(const std::vector<event>& events) const;
        void notify_overflow(const std::string& path) const;

        /*
         * This function filters the event types of an event leaving only the types
         * allowed by the configured filters.
         * */
        std::vector<fsw_event_flag> filter_flags(const event& evt) const;

        /*
         * This function implements the monitor event watching logic.  This function
         * is called from start() and it is executed on its thread.  This function
         * should block until the monitoring loop terminates: when it returns, the
         * monitor is marked as stopped.
         *
         * This function should cooperatively check the Monitor::should_stop field
         * locking monitor::run_mutex and return if set to true.
         * */
        virtual void run();

        /*
         * This function is executed by the stop() method, after requesting the
         * monitor to stop.  This handler is required if the thread running run() is
         * not able to preemptively stop its execution by checking the
         * Monitor::should_stop flag.
         * */
        virtual void on_stop();

    protected:
        std::vector<std::path> paths;
        std::map<std::string, std::string> properties;
        FM_EVENT_CALLBACK *callback;
        void *context = nullptr;
        double latency = 1.0;
        bool fire_idle_event = false;
        bool allow_overflow = false;
        bool recursive = false;
        bool follow_symlinks = false;
        bool directory_only = false;
        bool watch_access = false;

        /*
         * Monitor state
         * */
        bool running = false;
        bool should_stop = false;

        mutable std::mutex run_mutex;
        mutable std::mutex notify_mutex;

    private:
        std::chrono::milliseconds get_latency_ms() const;
        std::vector<COMPILED_MONITOR_FILTER_S> filters;
        std::vector<EVENT_TYPE_FILTER> event_type_filters;

        std::atomic<std::chrono::milliseconds> last_notification;
    };
}

#endif //FILE_MONITOR_MONITOR_H
