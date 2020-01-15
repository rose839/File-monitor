#include <sstream>
#include <ctime>
#include <cmath>
#include <set>
#include <map>
#include <sys/select.h>
#include <limits.h>
#include <unistd.h>
#include "exception.h"
#include "path_utils.h"
#include "monitor.h"
#include "log.h"
#include "inotify_monitor.h"

using namespace std;

namespace fm {

    struct inotify_monitor_impl {
        int inotify_monitor_handle = -1;
        std::vector<Event> events;

        set<int> watched_descriptors;
        map<string ,int> path_to_wd;

        map<int, string> wd_to_path;
        set<int> descriptors_to_remove;
        set<int> watches_to_remove;
        vector<string> paths_to_rescan;
        time_t curr_time;
    };

    static const unsigned int BUFFER_SIZE = (10 * 10 * ((sizeof(struct inotify_event)) + NAME_MAX + 1));

    Inotify_monitor::Inotify_monitor(std::vector <string> paths,
                                     FM_EVENT_CALLBACK *callback,
                                     void *context) :
       Monitor(paths, callback, context),
       impl(new inotify_monitor_impl())
                                     {
        impl->inotify_monitor_handle = inotify_init();
        if (impl->inotify_monitor_handle == -1) {
            perror("inotify init");
            throw fm_exception(string("Cannot initialize inotify."));
        }
    }

    Inotify_monitor::~Inotify_monitor() {
        for (auto inotify_desc_pair : impl->watched_descriptors) {
            if (inotify_rm_watch(impl->inotify_monitor_handle, inotify_desc_pair)) {
                perror("inotify_rm_watch");
            }
        }

        if (impl->inotify_monitor_handle > 0) {
            close(impl->inotify_monitor_handle);
        }
        delete impl;
    }

    bool Inotify_monitor::add_watch(const std::string &path, const struct stat &fd_stat) {
        int inotify_desc = inotify_add_watch(impl->inotify_monitor_handle,
                                            path.c_str(),
                                            IN_ALL_EVENTS);

        if (inotify_desc == -1) {
            perror("inotify_add_watch");
        } else {
            impl->watched_descriptors.insert(inotify_desc);
            impl->wd_to_path[inotify_desc] = path;
            impl->path_to_wd[path] = inotify_desc;
        }
        return (inotify_desc != -1);
    }

    void Inotify_monitor::scan(const std::string &path, const bool accept_non_dirs) {
        struct stat fd_stat;
        if (!lstat_path(path, fd_stat)) return;

        if (follow_symlinks && S_ISLNK(fd_stat.st_mode)) {
            string link_path;
            if (read_link_path(path, link_path)) {
                scan(link_path, accept_non_dirs);
            }
            return;
        }

        bool is_dir = S_ISDIR(fd_stat.st_mode);

        /*
         * When watching a directory the inotify API will return change events of
         * first-level children.  Therefore, we do not need to manually add a watch
         * for a child unless it is a directory.  By default, accept_non_dirs is
         * true to allow watching a file when first invoked on a node.
         *
         * For the same reason, the directory_only flag is ignored and treated as if
         * it were always set to true.
         */
        if (!is_dir && !accept_non_dirs) return; // not only accept dir
        if (!is_dir && directory_only) return;   // only directory
        if (!accept_path(path)) return;
        if (!add_watch(path, fd_stat)) return;
        if (!recursive || !is_dir) return;       // not recursive or not dir

        std::vector<std::string> children = get_directory_children(path);

        for (const std::string& child : children)
        {
            if (child == "." || child == "..") continue;

            /*
             * Scan children but only watch directories.
             */
            scan(path + "/" + child, false);
        }
    }

    bool Inotify_monitor::is_watched(const std::string &path) const {
        return (impl->path_to_wd.find(path) != impl->path_to_wd.end());
    }

    void Inotify_monitor::scan_root_paths() {
        for (string &path : paths) {
            if (!is_watched(path)) scan(path);
        }
    }

    void Inotify_monitor::preprocess_dir_event(struct inotify_event *event) {
        vector<fm_event_flag> flags;

        if (event->mask & IN_ISDIR) flags.push_back(fm_event_flag::IsDir);
        if (event->mask & IN_MOVE_SELF) flags.push_back(fm_event_flag::Updated);
        if (event->mask & IN_UNMOUNT) flags.push_back(fm_event_flag::PlatformSpecific);

        if (flags.size())
        {
            impl->events.push_back({impl->wd_to_path[event->wd], impl->curr_time, flags});
        }

        // If a new directory has been created, it should be rescanned if the
        if ((event->mask & IN_ISDIR) && (event->mask & IN_CREATE))
        {
            impl->paths_to_rescan.push_back(impl->wd_to_path[event->wd]);
        }
    }

    void Inotify_monitor::preprocess_node_event(struct inotify_event *event) {
        vector<fm_event_flag> flags;

        if (event->mask & IN_ACCESS) flags.push_back(fm_event_flag::PlatformSpecific);
        if (event->mask & IN_ATTRIB) flags.push_back(fm_event_flag::AttributeModified);
        if (event->mask & IN_CLOSE_NOWRITE) flags.push_back(fm_event_flag::PlatformSpecific);
        if (event->mask & IN_CLOSE_WRITE) flags.push_back(fm_event_flag::Updated);
        if (event->mask & IN_CREATE) flags.push_back(fm_event_flag::Created);
        if (event->mask & IN_DELETE) flags.push_back(fm_event_flag::Removed);
        if (event->mask & IN_MODIFY) flags.push_back(fm_event_flag::Updated);
        if (event->mask & IN_MOVED_FROM)
        {
            flags.push_back(fm_event_flag::Removed);
            flags.push_back(fm_event_flag::MovedFrom);
        }
        if (event->mask & IN_MOVED_TO)
        {
            flags.push_back(fm_event_flag::Created);
            flags.push_back(fm_event_flag::MovedTo);
        }
        if (event->mask & IN_OPEN) flags.push_back(fm_event_flag::PlatformSpecific);

        /* build the file name */
        ostringstream filename_stream;
        filename_stream << impl->wd_to_path[event->wd];

        if (event->len > 1)
        {
            filename_stream << "/";
            filename_stream << event->name;
        }

        if (flags.size())
        {
            impl->events.push_back({filename_stream.str(), impl->curr_time, flags});
        }

        /*
         * inotify automatically removes the watch of a watched item that has been
         * removed and posts an IN_IGNORED event after an IN_DELETE_SELF.
         */
        if (event->mask & IN_IGNORED)
        {
            impl->descriptors_to_remove.insert(event->wd);
        }

        /*
         * inotify sends an IN_MOVE_SELF event when a watched object is moved into
         * the same filesystem and keeps watching it.  Since its path has changed,
         * we remove the watch so that recreation is attempted at the next
         * iteration.
         *
         * Beware that a race condition exists which may result in events go
         * unnoticed when a watched file x is removed and a new file named x is
         * created thereafter.  In this case, fswatch could be blocked on read and
         * it would not have any chance to create a new watch descriptor for x until
         *  an event is received and read unblocks.
         */
        if (event->mask & IN_MOVE_SELF)
        {
            impl->watches_to_remove.insert(event->wd);
            impl->descriptors_to_remove.insert(event->wd);
        }

        /*
         * A file could be moved to a path which is being observed.  The clobbered
         * file is handled by the corresponding IN_DELETE_SELF event.
         */

        /*
         * inotify automatically removes the watch of the object the IN_DELETE_SELF
         * event is related to.
         */
        if (event->mask & IN_DELETE_SELF)
        {
            std::ostringstream log;
            log << "IN_DELETE_SELF: " << event->wd << "::" << filename_stream.str() << "\n";
            FM_ELOG(log.str().c_str());

            impl->descriptors_to_remove.insert(event->wd);
        }
    }

    void Inotify_monitor::preprocess_event(struct inotify_event *event)
    {
        if (event->mask & IN_Q_OVERFLOW)
        {
            notify_overflow(impl->wd_to_path[event->wd]);
        }

        preprocess_dir_event(event);
        preprocess_node_event(event);
    }

    void Inotify_monitor::remove_watch(int wd)
    {
        /*
         * No need to remove the inotify watch because it is removed automatically
         * when a watched element is deleted.
         */
        impl->wd_to_path.erase(wd);
    }

    void Inotify_monitor::process_pending_events()
    {
        // Remove watches.
        auto wtd = impl->watches_to_remove.begin();
        while (wtd != impl->watches_to_remove.end())
        {
            if (inotify_rm_watch(impl->inotify_monitor_handle, *wtd) != 0)
            {
                perror("inotify_rm_watch");
            }

            impl->watches_to_remove.erase(wtd++);
        }

        // Clean up descriptors.
        auto fd = impl->descriptors_to_remove.begin();
        while (fd != impl->descriptors_to_remove.end())
        {
            const std::string& curr_path = impl->wd_to_path[*fd];
            impl->path_to_wd.erase(curr_path);
            impl->wd_to_path.erase(*fd);
            impl->watched_descriptors.erase(*fd);

            impl->descriptors_to_remove.erase(fd++);
        }

        // Process paths to be rescanned
        std::for_each(impl->paths_to_rescan.begin(),
                      impl->paths_to_rescan.end(),
                      [this] (const std::string& p)
                      {
                          this->scan(p);
                      }
        );

        impl->paths_to_rescan.clear();
    }

    void Inotify_monitor::run()
    {
        char buffer[BUFFER_SIZE];
        double sec;
        double frac = modf(this->latency, &sec);

        for(;;)
        {
            std::unique_lock<std::mutex> run_guard(run_mutex);
            if (should_stop) break;
            run_guard.unlock();

            process_pending_events();

            scan_root_paths();

            // If no files can be watched, sleep and repeat the loop.
            if (!impl->watched_descriptors.size())
            {
                sleep(latency);
                continue;
            }

            // Use select to timeout on file descriptor read the amount specified by
            // the monitor latency.  This way, the monitor has a chance to update its
            // watches with at least the periodicity expected by the user.
            fd_set set;
            struct timeval timeout;

            FD_ZERO(&set);
            FD_SET(impl->inotify_monitor_handle, &set);
            timeout.tv_sec = sec;
            timeout.tv_usec = 1000 * 1000 * frac;

            int rv = select(impl->inotify_monitor_handle + 1,
                            &set,
                            nullptr,
                            nullptr,
                            &timeout);

            if (rv == -1) {
                continue;
            }

            // In case of read timeout just repeat the loop.
            if (rv == 0) continue;

            ssize_t record_num = read(impl->inotify_monitor_handle,
                                      buffer,
                                      BUFFER_SIZE);

            if (!record_num) {
                throw fm_exception(string("read() on inotify descriptor read 0 records."));
            }

            if (record_num == -1) {
                perror("read()");
                throw fm_exception(string("read() on inotify descriptor returned -1."));
            }

            time(&impl->curr_time);

            for (char *p = buffer; p < buffer + record_num;)
            {
                struct inotify_event *event = reinterpret_cast<struct inotify_event *> (p);

                preprocess_event(event);

                p += (sizeof(struct inotify_event)) + event->len;
            }

            if (impl->events.size())
            {
                notify_events(impl->events);
                impl->events.clear();
            }

            sleep(latency);
        }
    }
}
