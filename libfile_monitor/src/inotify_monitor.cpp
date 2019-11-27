#include <sstream>
#include <ctime>
#include <cmath>
#include <set>
#include <map>
#include <sys/select.h>
#include <limits.h>
#include "exception.h"
#include "path_utils.h"
#include "monitor.h"
#include "inotify_monitor.h"

using namespace std;

namespace fm {

    struct inotify_monitor_impl {
        int inotify_monitor_handle = -1;
        std::vector<event> events;

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

    }
}