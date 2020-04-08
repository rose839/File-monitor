#ifndef FMONITOR_H
#define FMONITOR_H

struct printf_event_callbacks {
	void (*format_f)(const event& evt);
	void (*format_p)(const event& evt);
	void (*format_t)(const event& evt);
};

/* exit error num */
#define FM_EXIT_OK              0
#define FM_EXIT_UNK_OPT         1
#define FM_EXIT_USAGE           2
#define FM_EXIT_LATENCY         3
#define FM_EXIT_STREAM          4
#define FM_EXIT_ERROR           5
#define FM_EXIT_ENFILE          6
#define FM_EXIT_OPT             7
#define FM_EXIT_MONITOR_NAME    8
#define FM_EXIT_FORMAT          9

#endif