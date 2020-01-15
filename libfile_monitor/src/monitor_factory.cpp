#include <vector>
#include <string>
#include "monitor_factory.h"
#include "error.h"
#include "event.h"
#include "exception.h"
#include "config.h"

#if defined(HAVE_SYS_INOTIFY_H)
#include "inotify_monitor.h"
#endif
#include "poll_monitor.h"

namespace fm {
    static Monitor *create_default_monitor(std::vector<std::string> paths,
                                           FM_EVENT_CALLBACK *callback,
                                           void *context) {
        fm_monitor_type type;

#if defined(HAVE_SYS_INOTIFY_H)
        type = fm_monitor_type::inotify_monitor_type;
#else
        type = fm_monitor_type::poll_monitor_type;
#endif
        return Monitor_factory::create_monitor(type,
                                               std::move(paths),
                                               callback,
                                               context);
    }

    Monitor *Monitor_factory::create_monitor(fm_monitor_type type,
                                             std::vector<std::string> paths,
                                             FM_EVENT_CALLBACK *callback,
                                             void *context)
    {
        switch (type)
        {
            case system_default_monitor_type:
                return create_default_monitor(paths, callback, context);

#if defined(HAVE_SYS_INOTIFY_H)
            case inotify_monitor_type:
        return new Inotify_monitor(paths, callback, context);
#endif
            case poll_monitor_type:
                return new Poll_monitor(paths, callback, context);
            default:
                throw fm_exception("Unsupported monitor.",
                                       FM_ERR_UNKNOWN_MONITOR_TYPE);
        }
    }

    std::map<std::string, fm_monitor_type>& Monitor_factory::creators_by_string()
    {
#define fm_quote(x) #x
        static std::map<std::string, fm_monitor_type> creator_by_string_set;


#if defined(HAVE_SYS_INOTIFY_H)
        creator_by_string_set[fm_quote(inotify_monitor)] = fm_monitor_type::inotify_monitor_type;
#endif

        creator_by_string_set[fm_quote(
                poll_monitor)] = fm_monitor_type::poll_monitor_type;

        return creator_by_string_set;
#undef fsw_quote
    }

    Monitor *Monitor_factory::create_monitor(const std::string& name,
                                             std::vector<std::string> paths,
                                             FM_EVENT_CALLBACK *callback,
                                             void *context)
    {
        auto i = creators_by_string().find(name);

        if (i == creators_by_string().end())
            return nullptr;

        return create_monitor(i->second, std::move(paths), callback, context);
    }

    bool Monitor_factory::exists_type(const std::string& name)
    {
        auto i = creators_by_string().find(name);

        return (i != creators_by_string().end());
    }

    std::vector<std::string> Monitor_factory::get_types()
    {
        std::vector<std::string> types;

        for (const auto& i : creators_by_string())
        {
            types.push_back(i.first);
        }

        return types;
    }
}
