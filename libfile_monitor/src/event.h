/*
 * @brief Header of the fm::event class.
 * */

#ifndef FILE_MONITOR_EVENT_H
#define FILE_MONITOR_EVENT_H

#include <string>
#include <ctime>
#include <vector>
#include <iostream>

namespace fm {
    enum fm_event_flag {
        NoOp = 0,                     /* No event has occurred. */
        PlatformSpecific = (1 << 0),  /* Platform-specific placeholder for event type that cannot currently be mapped. */
        Created = (1 << 1),           /* An object was created. */
        Updated = (1 << 2),           /* An object was updated. */
        Removed = (1 << 3),           /* An object was removed. */
        Renamed = (1 << 4),           /* An object was renamed. */
        OwnerModified = (1 << 5),     /* The owner of an object was modified. */
        AttributeModified = (1 << 6), /* The attributes of an object were modified. */
        MovedFrom = (1 << 7),         /* An object was moved from this location. */
        MovedTo = (1 << 8),           /* An object was moved to this location. */
        IsFile = (1 << 9),            /* The object is a file. */
        IsDir = (1 << 10),            /* The object is a directory. */
        IsSymLink = (1 << 11),        /* The object is a symbolic link. */
        Link = (1 << 12),             /* The link count of an object has changed. */
        Overflow = (1 << 13)          /* The event queue has overflowed. */
    };

    class Event {
    public:
        Event(std::string path, time_t event_time, std::vector<fm_event_flag> flags);
        virtual ~Event();

        std::string get_path() const;
        time_t get_time() const;
        std::vector<fm_event_flag> get_flags() const;

        /*
         * @brief Get event flag by name.
         *
         * @param name The name of the event flag to look for.
         * @return The event flag whose name is @p name.
         * @exception libfsw_exception if no event flag is found.
         * */
        static fm_event_flag get_event_flag_by_name(const std::string &name);

        /*
         * @brief Get the name of an event flag.
         *
         * @param flag The event flag.
         * @return The name of @p flag.
         * @exception libfsw_exception if no event flag is found.
         * */
        static std::string get_event_flag_name(const fm_event_flag &flag);

    private:
        std::string path;
        time_t event_time;
        std::vector<fm_event_flag> flags;
    };

    /*
     * @brief Overload of the "<<" operator to print an event flag
     * */
    std::ostream& operator<<(std::ostream &out, const fm_event_flag flag);
}

#endif //FILE_MONITOR_EVENT_H
