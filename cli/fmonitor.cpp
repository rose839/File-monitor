#include <iostream>
#include "fmonitor.h"
#include "config.h"

usint std::ostream;

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
	
}

int main(int argc, char **argv) {
	
}
