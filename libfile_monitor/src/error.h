#ifndef FILE_MONITOR_ERROR_H
#define FILE_MONITOR_ERROR_H

/*
 * Error codes
 * */
#define FM_OK                             0         /* The call was successful. */
#define FM_ERR_UNKNOWN_ERROR              (1 << 0)  /* An unknown error has occurred. */
#define FM_ERR_SESSION_UNKNOWN            (1 << 1)  /* The session specified by the handle is unknown. */
#define FM_ERR_MONITOR_ALREADY_EXISTS     (1 << 2)  /* The session already contains a monitor. */
#define FM_ERR_MEMORY                     (1 << 3)  /* An error occurred while invoking a memory management routine. */
#define FM_ERR_UNKNOWN_MONITOR_TYPE       (1 << 4)  /* The specified monitor type does not exist. */
#define FM_ERR_CALLBACK_NOT_SET           (1 << 5)  /* The callback has not been set. */
#define FM_ERR_PATHS_NOT_SET              (1 << 6)  /* The paths to watch have not been set. */
#define FM_ERR_MISSING_CONTEXT            (1 << 7)  /* The callback context has not been set. */
#define FSW_ERR_INVALID_PATH              (1 << 8)  /* The path is invalid. */
#define FSW_ERR_INVALID_CALLBACK          (1 << 9)  /* The callback is invalid. */
#define FSW_ERR_INVALID_LATENCY           (1 << 10) /* The latency is invalid. */
#define FSW_ERR_INVALID_REGEX             (1 << 11) /* The regular expression is invalid. */
#define FSW_ERR_MONITOR_ALREADY_RUNNING   (1 << 12) /* A monitor is already running in the specified session. */
#define FSW_ERR_UNKNOWN_VALUE             (1 << 13) /* The value is unknown. */
#define FSW_ERR_INVALID_PROPERTY          (1 << 14) /* The property is invalid. */


#endif //FILE_MONITOR_ERROR_H
