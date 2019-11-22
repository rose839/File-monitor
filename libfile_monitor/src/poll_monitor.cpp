#include <sys/stat.h>
#include <unistd.h>
#include <ctime>
#include "monitor.h"
#include "event.h"
#include "poll_monitor.h"

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

        WATCHED_FILE_INFO wfi{FM_MTIME(stat), FM_CTIME(stat)};
        previous_data->tracked_files[path] = wfi;

        return true;
    }

    bool Poll_monitor::intermediate_scan_callback(const std::string &path, const struct stat &fd_stat) {
        if (new_data->tracked_files.count(path)) return false;

        WATCHED_FILE_INFO wfi{FM_MTIME(stat), FM_CTIME(stat)};
        new_data->tracked_files[path] = wfi;

        if (previous_data->tracked_files.count(path)) {
            WATCHED_FILE_INFO pwfi = previous_data->tracked_files[path];
            vector<fm_event_flag> flags;

            if (FM_MTIME(stat) > pwfi.mtime) {
                flags.push_back(fm_event_flag::Updated);
            }

            if (FM_CTIME(stat) >pwfi.ctime) {
                flags.push_back(fm_event_flag::AttributeModified);
            }

            if (!flags.empty()) {
                events.emplace_back(path, curr_time, flags);
            }

            previous_data->tracked_files.erase(path);
        } else {
            vector<fsw_event_flag> flags;
            flags.push_back(fsw_event_flag::Created);
            events.emplace_back(path, curr_time, flags);
        }

        return true;
    }


}
