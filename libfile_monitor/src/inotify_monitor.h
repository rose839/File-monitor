
#ifndef FILE_MONITOR_INOTIFY_MONITOR_H
#define FILE_MONITOR_INOTIFY_MONITOR_H

#include <sys/inotify.h>
#include <string>
#include <vector>
#include <sys/stat.h>
#include "monitor.h"

namespace fm {
    struct inotify_monitor_impl;

    class Inotify_monitor : public Monitor {
    public:
        Inotify_monitor(std::vector<std::string> paths,
                        FM_EVENT_CALLBACK *callback,
                        void *context);
        virtual ~Inotify_monitor();

    protected:
        void run();

    private:
        Inotify_monitor(const Inotify_monitor &orig) = delete;
        Inotify_monitor &operator=(const Inotify_monitor &that) delete;

        void scan_root_paths();
        bool is_watched(const std::string &path) const;
        void preprocess_dir_event(struct inotify_event *event);
        void preprocess_event(struct inotify_event *event);
        void preprocess_node_event(struct inotify_event *event);
        void scan(const std::string &path, const bool accept_non_dirs = true);
        bool add_watch(const std::string &path,
                      const struct stat &fd_stat);
        void process_pending_events();
        void remove_watch(int fd);

        inotify_monitor_impl *impl;
    };
}

#endif //FILE_MONITOR_INOTIFY_MONITOR_H
