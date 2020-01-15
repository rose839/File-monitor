#include <sys/stat.h>
#include <unistd.h>
#include <ctime>
#include <string>
#include <vector>
#include <mutex>
#include "monitor.h"
#include "event.h"
#include "poll_monitor.h"
#include "path_utils.h"

using std::string;
using std::vector;
using std::unique_lock;
using std::mutex;

#define FM_MTIME(stat) ((stat).st_mtime)
#define FM_CTIME(stat) ((stat).st_ctime)

namespace fm {
    Poll_monitor::Poll_monitor(std::vector<string> paths,
                              FM_EVENT_CALLBACK *callback,
                               void *context) :
        Monitor(std::move(paths), callback, context)
    {
        previous_data = new POLL_MONITOR_DATA();
        new_data = new POLL_MONITOR_DATA();
        time(&curr_time);
    }

    Poll_monitor::~Poll_monitor() {
        delete previous_data;
        delete new_data;
    }

    bool Poll_monitor::initial_scan_callback(const std::string &path, const struct stat &fd_stat) {
        if (previous_data->tracked_files.count(path)) return false;

        WATCHED_FILE_INFO wfi{FM_MTIME(fd_stat), FM_CTIME(fd_stat)};
        previous_data->tracked_files[path] = wfi;

        return true;
    }

    bool Poll_monitor::intermediate_scan_callback(const std::string &path, const struct stat &fd_stat) {
        if (new_data->tracked_files.count(path)) return false;

        WATCHED_FILE_INFO wfi{FM_MTIME(fd_stat), FM_CTIME(fd_stat)};
        new_data->tracked_files[path] = wfi;

        if (previous_data->tracked_files.count(path)) {
            WATCHED_FILE_INFO pwfi = previous_data->tracked_files[path];
            vector<fm_event_flag> flags;

            if (FM_MTIME(fd_stat) > pwfi.mtime) {
                flags.push_back(fm_event_flag::Updated);
            }

            if (FM_CTIME(fd_stat) >pwfi.ctime) {
                flags.push_back(fm_event_flag::AttributeModified);
            }

            if (!flags.empty()) {
                events.emplace_back(path, curr_time, flags);
            }

            previous_data->tracked_files.erase(path);
        } else {
            vector<fm_event_flag> flags;
            flags.push_back(fm_event_flag::Created);
            events.emplace_back(path, curr_time, flags);
        }

        return true;
    }

    bool Poll_monitor::add_path(const std::string &path, const struct stat &fd_stat,
                                poll_monitor_scan_callback poll_callback) {
        return (this->*(poll_callback))(path, fd_stat);
    }

    void Poll_monitor::scan(const std::string &path,
                            poll_monitor_scan_callback fn) {
        struct stat fd_stat;
        if (!lstat_path(path, fd_stat))
            return;

        if (follow_symlinks && S_ISLNK(fd_stat.st_mode)) {
            string link_path;
            if (read_link_path(path, link_path))
                scan(link_path, fn);
        }

        if (!accept_path(path)) return;
        if (!add_path(path, fd_stat, fn)) return;
        if (!recursive) return;
        if (!S_ISDIR(fd_stat.st_mode)) return;

        vector<string> children = get_directory_children(path);
        for (const string &child : children) {
            if (child == "." || child == "..") continue;

            scan(path + "/" + child, fn);
        }
    }

    void Poll_monitor::find_removed_files() {
        vector<fm_event_flag> flags;
        flags.push_back(fm_event_flag::Removed);

        for (auto &removed : previous_data->tracked_files) {
            events.emplace_back(removed.first, curr_time, flags);
        }
    }

    void Poll_monitor::swap_data_containers() {
        delete  previous_data;
        previous_data = new_data;
        new_data = new POLL_MONITOR_DATA();
    }

    void Poll_monitor::collect_data() {
        poll_monitor_scan_callback fn = &Poll_monitor::intermediate_scan_callback;

        for (string &path : paths) {
            scan(path, fn);
        }

        find_removed_files();
        swap_data_containers();
    }

    void Poll_monitor::collect_initial_data() {
        poll_monitor_scan_callback fn = &Poll_monitor::initial_scan_callback;

        for (string &path : paths) {
            scan(path, fn);
        }
    }

    void Poll_monitor::run() {
        collect_initial_data();

        for (;;) {
            unique_lock<mutex> run_guard(run_mutex);
            if (should_stop) break;
            run_guard.unlock();

            sleep(latency < MIN_POLL_LATENCY ? MIN_POLL_LATENCY : latency);
            time(&curr_time);
            collect_data();

            if (!events.empty()) {
                notify_events(events);
                events.clear();
            }
        }
    }
}
