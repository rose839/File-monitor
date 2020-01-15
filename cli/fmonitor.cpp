#include <iostream>
#include <string>
#include "fmonitor.h"
#include "config.h"

using std::ostream;
using std::string;
using namespace fm;

/*
 * OPT_* variables are used as getopt_long values for long options that do not
 * have a short option equivalent.
 */
static const int OPT_BATCH_MARKER = 128;
static const int OPT_FORMAT = 129;
static const int OPT_EVENT_FLAG_SEPARATOR = 130;
static const int OPT_EVENT_TYPE = 131;
static const int OPT_ALLOW_OVERFLOW = 132;
static const int OPT_MONITOR_PROPERTY = 133;
static const int OPT_FIRE_IDLE_EVENTS = 134;
static const int OPT_FILTER_FROM = 135;

static void usage(ostream &stream) {
	stream << "\n\n";
	stream << "Usage:\n";
	stream << PACKAGE_NAME << " [option] ... path ...\n";
	stream << "\n";
	stream << "Options:\n";
	stream << " -0, --print0          " << "Use the ASCII NUL character (0) as line separator.\n";
	stream << " -1, --one-event       " << "Exit fswatch after the first set of events is received.\n";
	stream << "     --allow-overflow  " << "Allow a monitor to overflow and report it as a change event.\n";
	stream << "     --batch-marker    " << "Print a marker at the end of every batch.\n";
	stream << " -a, --access          " << "Watch file accesses.\n";
	stream << " -d, --directories     " << "Watch directories only.\n";
	stream << " -e, --exclude=REGEX   " << "Exclude paths matching REGEX.\n";
	stream << " -E, --extended        " << "Use extended regular expressions.\n";
	stream << "     --filter-from=FILE\n";
	stream << "                       " << "Load filters from file.\n";
	stream << "     --format=FORMAT   " << "Use the specified record format.\n";
	stream << " -f, --format-time     " << "Print the event time using the specified format.\n";
	stream << "     --fire-idle-event " << "Fire idle events.\n";
	stream << " -h, --help            " << "Show this message.\n";
	stream << " -i, --include=REGEX   " << "Include paths matching REGEX.\n";
	stream << " -I, --insensitive     " << "Use case insensitive regular expressions.\n";
	stream << " -l, --latency=DOUBLE  " << "Set the latency.\n";
	stream << " -L, --follow-links    " << "Follow symbolic links.\n";
	stream << " -M, --list-monitors   " << "List the available monitors.\n";
	stream << " -m, --monitor=NAME    " << "Use the specified monitor.\n";
	stream << "     --monitor-property name=value\n";
	stream << "                       " << "Define the specified property.\n";
	stream << " -n, --numeric         " << "Print a numeric event mask.\n";
	stream << " -o, --one-per-batch   " << "Print a single message with the number of change events.\n";
	stream << " -r, --recursive       " << "Recurse subdirectories.\n";
	stream << " -t, --timestamp       " << "Print the event timestamp.\n";
	stream << " -u, --utc-time        " << "Print the event time as UTC time.\n";
	stream << " -x, --event-flags     " << "Print the event flags.\n";
	stream << "     --event=TYPE      " << "Filter the event by the specified type.\n";
	stream << "     --event-flag-separator=STRING\n";
	stream << "                       " << "Print event flags using the specified separator.\n";
	stream << " -v, --verbose         " << "Print verbose output.\n";
	stream << "     --version         " << "Print the version of ") << PACKAGE_NAME << " and exit.\n";
	stream << "\n\n";
}

static void parse_opts(int argc, char **argv) {
	int ch;
  	string short_options = "01ade:Ef:hi:Il:LMm:nortuvx";
	int option_index = 0;
	static struct option long_options[] = {
	{"access",               no_argument,       nullptr,       'a'},
	{"allow-overflow",       no_argument,       nullptr,       OPT_ALLOW_OVERFLOW},
	{"batch-marker",         optional_argument, nullptr,       OPT_BATCH_MARKER},
	{"directories",          no_argument,       nullptr,       'd'},
	{"event",                required_argument, nullptr,       OPT_EVENT_TYPE},
	{"event-flags",          no_argument,       nullptr,       'x'},
	{"event-flag-separator", required_argument, nullptr,       OPT_EVENT_FLAG_SEPARATOR},
	{"exclude",              required_argument, nullptr,       'e'},
	{"extended",             no_argument,       nullptr,       'E'},
	{"filter-from",          required_argument, nullptr,       OPT_FILTER_FROM},
	{"fire-idle-events",     no_argument,       nullptr,       OPT_FIRE_IDLE_EVENTS},
	{"follow-links",         no_argument,       nullptr,       'L'},
	{"format",               required_argument, nullptr,       OPT_FORMAT},
	{"format-time",          required_argument, nullptr,       'f'},
	{"help",                 no_argument,       nullptr,       'h'},
	{"include",              required_argument, nullptr,       'i'},
	{"insensitive",          no_argument,       nullptr,       'I'},
	{"latency",              required_argument, nullptr,       'l'},
	{"list-monitors",        no_argument,       nullptr,       'M'},
	{"monitor",              required_argument, nullptr,       'm'},
	{"monitor-property",     required_argument, nullptr,       OPT_MONITOR_PROPERTY},
	{"numeric",              no_argument,       nullptr,       'n'},
	{"one-per-batch",        no_argument,       nullptr,       'o'},
	{"one-event",            no_argument,       nullptr,       '1'},
	{"print0",               no_argument,       nullptr,       '0'},
	{"recursive",            no_argument,       nullptr,       'r'},
	{"timestamp",            no_argument,       nullptr,       't'},
	{"utc-time",             no_argument,       nullptr,       'u'},
	{"verbose",              no_argument,       nullptr,       'v'},
	{"version",              no_argument,       &version_flag, true},
	{nullptr, 0,                                nullptr,       0}
	};

	
}

int main(int argc, char **argv) {
	
}
