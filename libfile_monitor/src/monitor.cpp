#include <mutex>
#include <thread>
#include <utility>
#include "monitor.h"
#include "exception.h"

namespace fm {
#define FM_MONITOR_RUN_GUARD std::unique_lock<std::mutex> run_guard(run_mutex);
#define FM_MONITOR_RUN_GUARD_LOCK run_guard.lock();
#define FM_MONITOR_RUN_GUARD_UNLOCK run_guard.unlock();

    Monitor::Monitor(std::vector <string> paths,
                     FM_EVENT_CALLBACK *callback,
                     void *context) :
                     paths(std::move(paths)), callback(callback), context(context), latency(1)
                     {
                        if (callback == nullptr) {
                            throw fm_exception("Callback ");
                        }
                     }
}

