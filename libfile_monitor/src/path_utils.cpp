#include <dirent.h>
#include <cstdlib>
#include <cstdio>
#include <errno.h>
#include <sys/types.h>
#include <iostream>
#include <system_error>
#include "path_utils.h"

using namespace std;

namespace fm {
    vector<string> get_directory_children(const string &path) {
        vector<string> children;
        DIR *dir = opendir(path.c_str());

        if (!dir) {
            if (errno == EMFILE || errno == ENFILE) {
                perror("opendir");
            }

            return children;
        }

        while (struct dirent *entry = readdir(dir)) {
            children.emplace_back(entry->d_name);
        }

        closedir(dir);
        return children;
    }

    bool read_link_path(const string &path, string &link_path) {
        link_path = fm_realpath(path.c_str(), nullptr);
		return true;
    }

    string fm_realpath(const char *path, char *resolved_path) {
        char *ret = realpath(path, resolved_path);

        if (ret == nullptr) {
            if (errno != ENOENT)
                throw system_error(errno, generic_category());
        }

        return string(ret);
    }

    bool stat_path(const string &path, struct stat &fd_stat) {
        if (stat(path.c_str(), &fd_stat) == 0)
            return true;
        return false;
    }

    bool lstat_path(const string &path, struct stat &fd_stat) {
        if (lstat(path.c_str(), &fd_stat) == 0)
            return true;
        return false;
    }
}
