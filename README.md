# File-monitor
A framework and a command tool for monitor directory and file.

## Install
cd File-monitor

mkdir build && cd build

cmake ..

make && make install

## Usage
### Command

```
Usage:
fmonitor [option] ... path ...

Options:
 -0, --print0          Use the ASCII NUL character (0) as line separator.
 -1, --one-event       Exit fswatch after the first set of events is received.
     --allow-overflow  Allow a monitor to overflow and report it as a change event.
     --batch-marker    Print a marker at the end of every batch.
 -a, --access          Watch file accesses.
 -d, --directories     Watch directories only.
 -e, --exclude=REGEX   Exclude paths matching REGEX.
 -E, --extended        Use extended regular expressions.
     --filter-from=FILE
                       Load filters from file.
     --format=FORMAT   Use the specified record format.
 -f, --format-time     Print the event time using the specified format.
     --fire-idle-event Fire idle events.
 -h, --help            Show this message.
 -i, --include=REGEX   Include paths matching REGEX.
 -I, --insensitive     Use case insensitive regular expressions.
 -l, --latency=DOUBLE  Set the latency.
 -L, --follow-links    Follow symbolic links.
 -M, --list-monitors   List the available monitors.
 -m, --monitor=NAME    Use the specified monitor.
     --monitor-property name=value
                       Define the specified property.
 -n, --numeric         Print a numeric event mask.
 -o, --one-per-batch   Print a single message with the number of change events.
 -r, --recursive       Recurse subdirectories.
 -t, --timestamp       Print the event timestamp.
 -u, --utc-time        Print the event time as UTC time.
 -x, --event-flags     Print the event flags.
     --event=TYPE      Filter the event by the specified type.
     --event-flag-separator=STRING
                       Print event flags using the specified separator.
 -v, --verbose         Print verbose output.
     --version         Print the version of fmonitor and exit.
```

### Lib
You can link libfile_monitor.so in your own programe, and expand your monitor functionality as descriped in monitor.h.

