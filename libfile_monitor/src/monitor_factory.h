#ifndef FILE_MONITOR_MONITOR_FACTORY_H
#define FILE_MONITOR_MONITOR_FACTORY_H

#include "monitor.h"

namespace fm{

    class Monitor_factory{
        /*
         * Creates a monitor of the specified type.
         *
         * The other parameters are forwarded to the fm::Monitor() constructor.
         * */
        static Moniter *create_monitor(fm_monitor_type type,
                                      std::vector<std::string> paths,
                                      FM_EVENT_CALLBACK *callback,
                                      void *context = nullptr);

        static monitor *create_monitor(const std::string& name,
                                       std::vector<std::string> paths,
                                       FSW_EVENT_CALLBACK *callback,
                                       void *context = nullptr);
        static std::vector<std::string> get_types();
        static bool exists_type(const std::string& name);

        Monitor_factory() = delete;
        Monitor_factory(const Monitor_factory &orig) = delete;
        Monitor_factory& operator=(const Monitor_factory) = delete;

    private:
        static std::map<std::string, fm_monitor_type> &creators_by_string();
    };
}

#endif //FILE_MONITOR_MONITOR_FACTORY_H
