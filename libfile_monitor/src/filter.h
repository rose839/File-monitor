/*
 * @brief Header of the fm::monitor_filter class.
 *
 * This header file defines the fm::monitor_filter class, a type that
 * represents a path filter.
 * */

#ifndef FILE_MONITOR_FILTER_H
#define FILE_MONITOR_FILTER_H

#include <string>
#include <vector>
#include "event.h"
#include "filter.h"

#define handle_error(t) if (err_handler) err_handler(t)

/*
 * @brief Path filters used to accept or reject file change events.
 *  A path filter is a regular expression used to accept or reject file change
 *  events based on the value of their path.  A filter has the following
 * characteristics:
 *
 *   - It has a _regular expression_ (monitor_filter::text), used to match the
 *     paths.
 *
 *   - It can be an _inclusion_ or an _exclusion_ filter
 *     (monitor_filter::type).
 *
 *   - It can be case _sensitive_ or _insensitive_
 *     (monitor_filter::case_sensitive).
 *
 *   - It can be an _extended_ regular expression (monitor_filter::extended).
 *
 * */
namespace fm{
    enum fm_filter_type {
        filter_include,
        filter_exclude
    };

    class Monitor_filter {
    public:
        std::string text;
        fm_filter_type type;
        bool case_sensitive;
        bool extended;

        /*
         * @brief Load filters from the specified file.
         *
         *Filters can be loaded from a text file containing one filter per line.
         * A filter has the following structure:
         *
         *   - It is validated by the following regular expression:
         *     `^([+-])([ei]*) (.+)$`
         *
         *   - The first character is the filter type: `+` if it is an _inclusion_
         *     filter, `-` if it is an _exclusion_ filter.
         *
         *   - An optional list of flags:
         *
         *     - `e` if it is an _extended_ regular expression.
         *
         *     - `i` if it is a _case insensitive_ regular expression.
         *
         *   - A space.
         *
         *   - The filter regular expression text.
         *
         * Parsing errors are notified through an optional error handler.  The valid
         * filters are returned in a vector.
         * */
        static std::vector<Monitor_filter> read_from_file(const std::string& path,
                                                          void (*err_handler)(std::string) = nullptr);
    };

    /*
     * Used to filter events by type.
     * */
    typedef struct _event_type_filter {
        enum fm_event_flag flag;
    }EVENT_TYPE_FILTER;
}

#endif //FILE_MONITOR_FILTER_H
