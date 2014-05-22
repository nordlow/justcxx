#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "ioredirect.hpp"

/*! Duplicate File Descriptor.
 * \return whatever \c dup2 or \c dup3 returns.
 * \todo Better to use \c freopen?
 */
inline int dupx(int oldfd, int newfd)
{
#ifdef _GNU_SOURCE
    int flags = 0;
    return dup3(oldfd, newfd, flags);
#else
    return dup2(oldfd, newfd);
#endif
}

/*! Create Pipe.
 * \return whatever \c pipe or \c pipe2 returns.
 */
inline int pipen(int pipefd[2], bool async = true)
{
#ifdef _GNU_SOURCE
    return pipe2(pipefd, async ? O_NONBLOCK : 0);
#else
    const int p = pipe(pipefd);
    if (async) { fcntl(p, O_NONBLOCK); }
    return p;
#endif
}


/*! \todo Create variant using Boost.Process found at http://www.highscore.de/boost/process0.5/boost_process/tutorial.html
  */
int
spawn(const char* filename,
      const char* working_directory,
      char *const argv[],
      char *const envp[],
      pid_t *child_pid,
      int *standard_input,
      int *standard_output,
      int *standard_error)
{
    int   p_in[2];              // \c child stdin pipe
    int   p_out[2];             // \c child stdout pipe
    int   p_err[2];             // \c child stderr pipe

    const bool async = true; // non-blocking pipe

    if (pipen(p_in, async) < 0 or
        pipen(p_out, async) < 0 or
        pipen(p_err, async) < 0) {
        perror("pipe");
        return -1;
    }

    pid_t pid;                  // child process id (PID)
    if ((pid = fork()) < 0) {
        perror("fork");
    } else if (pid > 0) {       // executing in (supervising) parent task
        close(p_in[0]);         // parent does no \c stdin reads
        close(p_out[1]);        // parent does no \c stdout writes
        close(p_err[1]);        // parent does no \c stdout writes
    } else {			// executing in \c filename child task
        close(p_in[1]);         // child does no \c stdin writes
        close(p_out[0]);        // child does no \c stdout reads
        close(p_err[0]);        // child does no \c stderr reads

        // convenient aliases
        int& c_in  = p_in[0];  // child \c stdin
        int& c_out = p_out[1]; // child \c stdout
        int& c_err = p_err[1]; // child \c stderr

        // enter directory
        if (working_directory) {
            if (chdir(working_directory) < 0) {
                close(c_in); close(c_out); close(c_err);
                perror(working_directory); exit(-1);
            }
        }

        // redirect \c stdin to \c c_in
        if (c_in != STDIN_FILENO) {
            if (dupx(c_in, STDIN_FILENO) != STDIN_FILENO) {
                perror("dup"); close(c_in);
            }
        }

        // redirect \c stdout to \c c_out
        if (c_out != STDOUT_FILENO) {
            if (dupx(c_out, STDOUT_FILENO) != STDOUT_FILENO) {
                perror("dup"); close(c_out);
            }
        }

        // redirect \c stderr to \c c_err
        if (c_err != STDERR_FILENO) {
            if (dupx(c_err, STDERR_FILENO) != STDERR_FILENO) {
                perror("dup"); close(c_err);
            }
        }

        // execute
        int exit_status = execve(filename, argv, envp);
        if (exit_status < 0) {
            perror("execve");
        }
        exit(exit_status);

    }

    if (child_pid) { *child_pid = pid; }
    if (standard_input) { *standard_input = p_in[1]; }
    if (standard_output) { *standard_output = p_out[0]; }
    if (standard_error) { *standard_error = p_err[0]; }
    return 0;
}
