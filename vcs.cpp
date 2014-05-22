#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <iomanip>
#include <string>

#include "vcs.hpp"
#include "pathops.hpp"
#include "find_exec.hpp"
#include "ioredirect.hpp"
#include "timing.h"
#include "stdio_x.h"

/*! \em Require (On-Demand Auto-Install) Packages Providing Command/Executable \p cmd. */
template<class Ch>
inline std::basic_string<Ch> require(const Ch* cmd) {
    // char wdb[PATH_MAX];     // working directory buffer
    // auto wd = ::getcwd(wdb, sizeof(wdb));
    if (is_absolute_path(cmd)) {
        return cmd;
    } else if (is_relative_path(cmd)) {
        return cmd;
    } else if (is_local_path(cmd)) {
        return find_executable_by_name(cmd);
    } else {
        return cmd;
    }
 }

/*! \em Require (On-Demand Auto-Install) Packages Providing Command/Executable
 * \p cmd and then \em spawn a separate process using
 * \p cmd together with it arguments \p args.
 */
template<class...R> int auto_required_spawn(const char* cmd, R&&...args) {
    return spawn(require(cmd).c_str(), args...);
}
template<class Ch, class...R> int auto_required_spawn(const std::basic_string<Ch> cmd, R&&...args) {
    return spawn(require(cmd.c_str()).c_str(), args...);
}

int vcs_status(std::vector<VCS_STATUS_t>& status_list_return,
               const char * dir,
               VCS_t vcs)
{
    int ret = -1;
    pTimer tmr; ptimer_tic(&tmr);

    if (vcs == VCS_undefined_) {
        // TODO: use SemNet pPatts to figure out \c vcs from contents of current directory (\c Dir) and matchers for these.
        vcs = VCS_GIT;
    }
    pid_t child = -1;           // child process
    int out, err;
    const char * envp[] = { nullptr };
    const char * cmd = nullptr;
    const char * op = "status";
    const char * flag = "-S";
    switch (vcs) {
    case VCS_CVS: cmd = "cvs"; break;
    case VCS_SVN: cmd = "svn"; break;
    case VCS_GIT: cmd = "git"; flag = "-s"; break;
    case VCS_BZR: cmd = "bzr"; break;
    case VCS_HG:  cmd = "hg";  break;
    case VCS_DARCS: cmd = "darcs"; op = "show"; break;
    default: break;
    }
    const char * argv[] = { cmd, op, flag, nullptr };
    ret = auto_required_spawn(find_executable_by_name(cmd).c_str(), dir,
                              const_cast<char* const*>(argv), const_cast<char* const*>(envp),
                              &child, nullptr, &out, &err);

    int status;
    waitpid(child, &status, 0); // wait for process to terminate
    std::cout << "status:" << status << std::endl;

    ptimer_toc(&tmr);
    ptimer_print_sec_usec9(tmr); endline();

    int rc, c;
    // read out
    std::cout << "out:" << std::endl;
    while ((rc = read(out, &c, 1)) > 0) { std::cout << char(c); }

    // read err
    std::cerr << "err:" << std::endl;
    while ((rc = read(err, &c, 1)) > 0) { std::cout << char(c); }

    return ret;
}
