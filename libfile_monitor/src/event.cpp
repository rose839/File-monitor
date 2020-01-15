
#include <map>
#include "exception.h"
#include "event.h"

namespace fm {
    Event::Event(std::string path, time_t event_time, std::vector <fm_event_flag> flags) :
        path(std::move(path)), event_time(event_time), flags(std::move(flags))
    {
    }

    Event::~Event() {

    }

    std::string Event::get_path() const {
        return path;
    }

    std::vector<fm_event_flag> Event::get_flags() const {
        return flags;
    }

    time_t Event::get_time() const {
        return event_time;
    }

    fm_event_flag Event::get_event_flag_by_name(const std::string &name) {
#define FM_MAKE_PAIR_FROM_NAME(p) {#p, p}
        static const std::map<std::string, fm_event_flag> flag_by_names = {
                FM_MAKE_PAIR_FROM_NAME(NoOp),
                FM_MAKE_PAIR_FROM_NAME(PlatformSpecific),
                FM_MAKE_PAIR_FROM_NAME(Created),
                FM_MAKE_PAIR_FROM_NAME(Updated),
                FM_MAKE_PAIR_FROM_NAME(Removed),
                FM_MAKE_PAIR_FROM_NAME(Renamed),
                FM_MAKE_PAIR_FROM_NAME(OwnerModified),
                FM_MAKE_PAIR_FROM_NAME(AttributeModified),
                FM_MAKE_PAIR_FROM_NAME(MovedFrom),
                FM_MAKE_PAIR_FROM_NAME(MovedTo),
                FM_MAKE_PAIR_FROM_NAME(IsFile),
                FM_MAKE_PAIR_FROM_NAME(IsDir),
                FM_MAKE_PAIR_FROM_NAME(IsSymLink),
                FM_MAKE_PAIR_FROM_NAME(Link),
                FM_MAKE_PAIR_FROM_NAME(Overflow)
        };
#undef FM_MAKE_PAIR_FROM_NAME

        auto flag = flag_by_names.find(name);
        if (flag == flag_by_names.end())
            throw fm_exception(std::string("Unknown event type: ") + name, FM_ERR_UNKNOWN_VALUE);
        return flag->second;
    }

    string Event::get_event_flag_name(const fm_event_flag& flag)
    {
#define FM_MAKE_PAIR_FROM_NAME(p) {p, #p}
        static const std::map<fm_event_flag, string> names_by_flag = {
                FM_MAKE_PAIR_FROM_NAME(NoOp),
                FM_MAKE_PAIR_FROM_NAME(PlatformSpecific),
                FM_MAKE_PAIR_FROM_NAME(Created),
                FM_MAKE_PAIR_FROM_NAME(Updated),
                FM_MAKE_PAIR_FROM_NAME(Removed),
                FM_MAKE_PAIR_FROM_NAME(Renamed),
                FM_MAKE_PAIR_FROM_NAME(OwnerModified),
                FM_MAKE_PAIR_FROM_NAME(AttributeModified),
                FM_MAKE_PAIR_FROM_NAME(MovedFrom),
                FM_MAKE_PAIR_FROM_NAME(MovedTo),
                FM_MAKE_PAIR_FROM_NAME(IsFile),
                FM_MAKE_PAIR_FROM_NAME(IsDir),
                FM_MAKE_PAIR_FROM_NAME(IsSymLink),
                FM_MAKE_PAIR_FROM_NAME(Link),
                FM_MAKE_PAIR_FROM_NAME(Overflow)
        };
#undef FM_MAKE_PAIR_FROM_NAME

        auto name = names_by_flag.find(flag);

        if (name == names_by_flag.end())
            throw fm_exception(std::string("Unknown event type."), FM_ERR_UNKNOWN_VALUE);

        return name->second;
    }

    std::ostream &operator<<(std::ostream &os, const fm_event_flag flag) {
        return os << Event::get_event_flag_name(flag);
    }
}

