#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <signal.h>

#include "ioredirect.hpp"
#include "vcs.hpp"

inline void sigpipe(int signo)
{
    printf("SIGPIPE caught with signo %d\n", signo);
}

int main(int argc, char * argv[])
{
    if (signal(SIGPIPE, sigpipe) == SIG_ERR) {
        perror("signal"); return -1;
    }

    std::vector<VCS_STATUS_t> status;
    vcs_status(status, "~/cognia/");
    // vcs_status(status, "/home/per/ware/emacs/");
    // vcs_status(status, "/home/per/ware/julia");
    // vcs_status(status, "~/ware/ctags");

    return 0;
}
