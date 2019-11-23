
#ifndef FILE_MONITOR_PATH_UTILS_H
#define FILE_MONITOR_PATH_UTILS_H

#include <string>
#include <vector>
#include <sys/stat.h>

namespace fm {
    std::string fm_realpath(const char *path, char *resolved_path);

    /* Gets a vector of direct directory children. */
    std::vector<std::string> get_directory_children(const std::string &path);
    bool read_link_path(const std::string &path, std::string &link_path);
    bool lstat_path(const std::string &path, struct stat &fd_stat);
    bool stat_path(const std::string &path, struct stat &fd_stat);
}

#endif //FILE_MONITOR_PATH_UTILS_H
