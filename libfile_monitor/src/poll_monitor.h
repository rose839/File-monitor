/**
 *  `stat()` based monitor.
 */

#ifndef FILE_MONITOR_POLL_MONITOR_H
#define FILE_MONITOR_POLL_MONITOR_H

#include <unordered_map>
#include <sys/stat.h>
#include <ctime>
#include "monitor.h"

namespace fm {
    class Poll_monitor : public Monitor {
    public:
        Poll_monitor(std::vector<std::string> paths,
                     FM_EVENT_CALLBACK *callback,
                     void *context = nullptr);
        virtual ~Poll_monitor();

    protected:
        void run();

    private:
        static const unsigned int MIN_POLL_LATENCY = 1;

        Poll_monitor(const Poll_monitor &orig) = delete;
        Poll_monitor &operator=(const Poll_monitor &orig) = delete;

        typedef bool (Poll_monitor::*poll_monitor_scan_callback)(
                const std::string &path,
                const struct stat &stat);

        typedef struct _watched_file_info {
            time_t mtime;
            time_t ctime;
        }WATCHED_FILE_INFO;

        typedef struct _poll_monitor_data {
            std::unordered_map<std::string, WATCHED_FILE_INFO> tracked_files;
        }POLL_MONITOR_DATA;

        void scan(const std::string &path, poll_monitor_scan_callback fn);
        void collect_initial_data();
        void collect_data();
        bool add_path(const std::string &path,
                      const struct stat &fd_stat,
                      poll_monitor_scan_callback poll_callback);
        bool initial_scan_callback(const std::string &path,
                                  const struct stat &fd_stat);
        bool intermediate_scan_callback(const std::string &path,
                                        const struct stat &fd_stat);

        void find_removed_files();
        void swap_data_containers();

        POLL_MONITOR_DATA *previous_data;
        POLL_MONITOR_DATA *new_data;

        std::vector<event> events;
        time_t curr_time;
    };
}

#endif //FILE_MONITOR_POLL_MONITOR_H
