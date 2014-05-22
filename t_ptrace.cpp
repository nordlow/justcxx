/*! \file f.cpp
 * \brief Test \c ptrace on Linux.
 *
 * \see http://pastie.org/3634410#47,55
 * \see http://pramode.net/articles/lfy/ptrace/pramode.html
 * \see http://stackoverflow.com/questions/9786648/using-ptrace-to-track-the-location-of-files-being-opened
 * \see http://www.linuxjournal.com/article/6210
 * \see http://gnurbs.blogsome.com/2006/06/24/anti-anti-ptrace-via-shared-library-function-overloading-in-c/
 * \see https://mikecvet.wordpress.com/2010/08/14/ptrace-tutorial/
 * \see http://www.math.leidenuniv.nl/~rhuizer/libptrace.pdf
 * \see http://stackoverflow.com/questions/6604007/how-can-i-get-a-list-of-linux-system-calls-and-number-of-args-they-take-automati
 * \see http://www.math.leidenuniv.nl/~rhuizer/libptrace.pdf
 * \see http://software.intel.com/sites/default/files/m/a/1/e/dsohowto.pdf
 * \see http://bluemaster.iu.hio.no/edu/dark/lin-asm/syscalls.html
 */

#include <cstdio>
#include <cstdlib>
#include <cstdbool>

#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ptrace.h>
#include <bfd.h>
// #include "/usr/src/linux-headers-3.5.0-26/arch/x86/include/asm/user.h"
#include <linux/user.h>

#include "show.hpp"
#include "enforce.hpp"
// #include <boost/lockfree/queue.hpp>
// #include <boost/lockfree/spsc_queue.hpp>

extern "C" {
// #include "strace/defs.h"
}

#define WORDSIZE (sizeof (long)) ///< Target Platform Word Size in bytes

#define EBX 0                   ///< Word Offset
#define ECX 1                   ///< Word Offset
#define EDX 2                   ///< Word Offset
#define ESI 3                   ///< Word Offset
#define EDI 4                   ///< Word Offset

#define TRACE_FILE	001	/* Trace file-related syscalls. */
#define TRACE_IPC	002	/* Trace IPC-related syscalls. */
#define TRACE_NETWORK	004	/* Trace network-related syscalls. */
#define TRACE_PROCESS	010	/* Trace process-related syscalls. */
#define TRACE_SIGNAL	020	/* Trace signal-related syscalls. */
#define TRACE_DESC	040	/* Trace file descriptor-related syscalls. */
#define TRACE_MEMORY	0100	/* Trace memory mapping-related syscalls. */
#define SYSCALL_NEVER_FAILS	0200	/* Syscall is always successful. */

/* Define these shorthand notations to simplify the syscallent files. */
#define TD TRACE_DESC
#define TF TRACE_FILE
#define TI TRACE_IPC
#define TN TRACE_NETWORK
#define TP TRACE_PROCESS
#define TS TRACE_SIGNAL
#define TM TRACE_MEMORY
#define NF SYSCALL_NEVER_FAILS
#define MA MAX_ARGS

/* Trace Control Block */
struct tcb {
    int flags;		/* See below for TCB_ values */
    int pid;		/* Process Id of this entry */
    int qual_flg;		/* qual_flags[scno] or DEFAULT_QUAL_FLAGS + RAW */
    int u_error;		/* Error code */
    long scno;		/* System call number */
//     long u_arg[MAX_ARGS];	/* System call arguments */
// #if defined(LINUX_MIPSN32) || defined(X32)
//     long long ext_arg[MAX_ARGS];
//     long long u_lrval;	/* long long return value */
// #endif
//     long u_rval;		/* Return value */
// #if SUPPORTED_PERSONALITIES > 1
//     int currpers;		/* Personality at the time of scno update */
// #endif
//     int curcol;		/* Output column for this process */
//     FILE *outf;		/* Output file for this process */
//     const char *auxstr;	/* Auxiliary info from syscall (see RVAL_STR) */
//     const struct_sysent *s_ent; /* sysent[scno] or dummy struct for bad scno */
//     struct timeval stime;	/* System time usage as of last process wait */
//     struct timeval dtime;	/* Delta for system time usage */
//     struct timeval etime;	/* Syscall entry time */
// 				/* Support for tracing forked processes: */
//     long inst[2];		/* Saved clone args (badly named) */
};

/* TCB flags */
#define TCB_INUSE		00001	/* This table entry is in use */
/* We have attached to this process, but did not see it stopping yet */
#define TCB_STARTUP		00002
#define TCB_IGNORE_ONE_SIGSTOP	00004	/* Next SIGSTOP is to be ignored */
/*
 * Are we in system call entry or in syscall exit?
 *
 * This bit is set after all syscall entry processing is done.
 * Therefore, this bit will be set when next ptrace stop occurs,
 * which should be syscall exit stop. Other stops which are possible
 * directly after syscall entry (death, ptrace event stop)
 * are simpler and handled without calling trace_syscall(), therefore
 * the places where TCB_INSYSCALL can be set but we aren't in syscall stop
 * are limited to trace(), this condition is never observed in trace_syscall()
 * and below.
 * The bit is cleared after all syscall exit processing is done.
 * User-generated SIGTRAPs and post-execve SIGTRAP make it necessary
 * to be very careful and NOT set TCB_INSYSCALL bit when they are encountered.
 * TCB_WAITEXECVE bit is used for this purpose (see below).
 *
 * Use entering(tcp) / exiting(tcp) to check this bit to make code more readable.
 */
#define TCB_INSYSCALL	00010
#define TCB_ATTACHED	00020   /* It is attached already */
/* Are we PROG from "strace PROG [ARGS]" invocation? */
#define TCB_STRACE_CHILD 0040
#define TCB_BPTSET	00100	/* "Breakpoint" set after fork(2) */
#define TCB_REPRINT	00200	/* We should reprint this syscall on exit */
#define TCB_FILTERED	00400	/* This system call has been filtered out */
/* x86 does not need TCB_WAITEXECVE.
 * It can detect post-execve SIGTRAP by looking at eax/rax.
 * See "not a syscall entry (eax = %ld)\n" message.
 *
 * Note! On new kernels (about 2.5.46+), we use PTRACE_O_TRACEEXEC, which
 * suppresses post-execve SIGTRAP. If you are adding a new arch which is
 * only supported by newer kernels, you most likely don't need to define
 * TCB_WAITEXECVE!
 */

#define entering(tcp)	(!((tcp)->flags & TCB_INSYSCALL))
#define exiting(tcp)	((tcp)->flags & TCB_INSYSCALL)
#define syserror(tcp)	((tcp)->u_error != 0)
#define verbose(tcp)	((tcp)->qual_flg & QUAL_VERBOSE)
#define abbrev(tcp)	((tcp)->qual_flg & QUAL_ABBREV)
#define filtered(tcp)	((tcp)->flags & TCB_FILTERED)

typedef struct sysent {
    unsigned nargs;
    int	sys_flags;
    int	(*sys_func)(struct tcb *tcp);
    const char *sys_name;
} struct_sysent;

typedef struct ioctlent {
    const char *doth;
    const char *symbol;
    unsigned long code;
} struct_ioctlent;

static int sys_generic(struct tcb *tcp) {
    printf("flags:%xd\n", tcp->flags);
    return 0;
}

static int sys_read(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_read,		"read"		},  /* 0 */
static int sys_write(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_write,		"write"		},  /* 1 */
static int sys_open(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD|TF,	sys_open,		"open"		},  /* 2 */
static int sys_close(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_close,		"close"		},  /* 3 */
static int sys_stat(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_stat,		"stat"		},  /* 4 */
static int sys_fstat(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_fstat,		"fstat"		},  /* 5 */
static int sys_lstat(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_lstat,		"lstat"		},  /* 6 */
static int sys_poll(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_poll,		"poll"		},  /* 7 */
static int sys_lseek(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_lseek,		"lseek"		},  /* 8 */
static int sys_mmap(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	TD|TM,	sys_mmap,		"mmap"		},  /* 9 */
static int sys_mprotect(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TM,	sys_mprotect,		"mprotect"	},  /* 10 */
static int sys_munmap(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TM,	sys_munmap,		"munmap"	},  /* 11 */
static int sys_brk(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TM,	sys_brk,		"brk"		},  /* 12 */
static int sys_rt_sigaction(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TS,	sys_rt_sigaction,	"rt_sigaction"	},  /* 13 */
static int sys_rt_sigprocmask(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TS,	sys_rt_sigprocmask,	"rt_sigprocmask"},  /* 14 */
static int sys_rt_sigreturn(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	TS,	sys_rt_sigreturn,	"rt_sigreturn"	},  /* 15 */
static int sys_ioctl(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_ioctl,		"ioctl"		},  /* 16 */
static int sys_pread(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD,	sys_pread,		"pread"		},  /* 17 */
static int sys_pwrite(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD,	sys_pwrite,		"pwrite"	},  /* 18 */
static int sys_readv(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_readv,		"readv"		},  /* 19 */
static int sys_writev(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_writev,		"writev"	},  /* 20 */
static int sys_access(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_access,		"access"	},  /* 21 */
static int sys_pipe(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_pipe,		"pipe"		},  /* 22 */
static int sys_select(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD,	sys_select,		"select"	},  /* 23 */
static int sys_sched_yield(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	0,	sys_sched_yield,	"sched_yield"	},  /* 24 */
static int sys_mremap(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TM,	sys_mremap,		"mremap"	},  /* 25 */
static int sys_msync(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TM,	sys_msync,		"msync"		},  /* 26 */
static int sys_mincore(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TM,	sys_mincore,		"mincore"	},  /* 27 */
static int sys_madvise(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TM,	sys_madvise,		"madvise"	},  /* 28 */
static int sys_shmget(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TI,	sys_shmget,		"shmget"	},  /* 29 */
static int sys_shmat(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TI,	sys_shmat,		"shmat"		},  /* 30 */
static int sys_shmctl(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TI,	sys_shmctl,		"shmctl"	},  /* 31 */
static int sys_dup(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_dup,		"dup"		},  /* 32 */
static int sys_dup2(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_dup2,		"dup2"		},  /* 33 */
static int sys_pause(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	TS,	sys_pause,		"pause"		},  /* 34 */
static int sys_nanosleep(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_nanosleep,		"nanosleep"	},  /* 35 */
static int sys_getitimer(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_getitimer,		"getitimer"	},  /* 36 */
static int sys_alarm(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_alarm,		"alarm"		},  /* 37 */
static int sys_setitimer(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_setitimer,		"setitimer"	},  /* 38 */
static int sys_getpid(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	0,	sys_getpid,		"getpid"	},  /* 39 */
static int sys_sendfile64(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD|TN,	sys_sendfile64,		"sendfile"	},  /* 40 */
static int sys_socket(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TN,	sys_socket,		"socket"	},  /* 41 */
static int sys_connect(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TN,	sys_connect,		"connect"	},  /* 42 */
static int sys_accept(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TN,	sys_accept,		"accept"	},  /* 43 */
static int sys_sendto(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	TN,	sys_sendto,		"sendto"	},  /* 44 */
static int sys_recvfrom(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	TN,	sys_recvfrom,		"recvfrom"	},  /* 45 */
static int sys_sendmsg(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TN,	sys_sendmsg,		"sendmsg"	},  /* 46 */
static int sys_recvmsg(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TN,	sys_recvmsg,		"recvmsg"	},  /* 47 */
static int sys_shutdown(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TN,	sys_shutdown,		"shutdown"	},  /* 48 */
static int sys_bind(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TN,	sys_bind,		"bind"		},  /* 49 */
static int sys_listen(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TN,	sys_listen,		"listen"	},  /* 50 */
static int sys_getsockname(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TN,	sys_getsockname,	"getsockname"	},  /* 51 */
static int sys_getpeername(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TN,	sys_getpeername,	"getpeername"	},  /* 52 */
static int sys_socketpair(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TN,	sys_socketpair,		"socketpair"	},  /* 53 */
static int sys_setsockopt(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TN,	sys_setsockopt,		"setsockopt"	},  /* 54 */
static int sys_getsockopt(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TN,	sys_getsockopt,		"getsockopt"	},  /* 55 */
static int sys_clone(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TP,	sys_clone,		"clone"		},  /* 56 */
static int sys_fork(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	TP,	sys_fork,		"fork"		},  /* 57 */
static int sys_vfork(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	TP,	sys_vfork,		"vfork"		},  /* 58 */
static int sys_execve(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TF|TP,	sys_execve,		"execve"	},  /* 59 */
static int sys_exit(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TP,	sys_exit,		"_exit"		},  /* 60 */
static int sys_wait4(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TP,	sys_wait4,		"wait4"		},  /* 61 */
static int sys_kill(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TS,	sys_kill,		"kill"		},  /* 62 */
static int sys_uname(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_uname,		"uname"		},  /* 63 */
static int sys_semget(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TI,	sys_semget,		"semget"	},  /* 64 */
static int sys_semop(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TI,	sys_semop,		"semop"		},  /* 65 */
static int sys_semctl(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TI,	sys_semctl,		"semctl"	},  /* 66 */
static int sys_shmdt(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TI,	sys_shmdt,		"shmdt"		},  /* 67 */
static int sys_msgget(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TI,	sys_msgget,		"msgget"	},  /* 68 */
static int sys_msgsnd(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TI,	sys_msgsnd,		"msgsnd"	},  /* 69 */
static int sys_msgrcv(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TI,	sys_msgrcv,		"msgrcv"	},  /* 70 */
static int sys_msgctl(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TI,	sys_msgctl,		"msgctl"	},  /* 71 */
static int sys_fcntl(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_fcntl,		"fcntl"		},  /* 72 */
static int sys_flock(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_flock,		"flock"		},  /* 73 */
static int sys_fsync(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_fsync,		"fsync"		},  /* 74 */
static int sys_fdatasync(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_fdatasync,		"fdatasync"	},  /* 75 */
static int sys_truncate(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_truncate,		"truncate"	},  /* 76 */
static int sys_ftruncate(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_ftruncate,		"ftruncate"	},  /* 77 */
static int sys_getdents(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_getdents,		"getdents"	},  /* 78 */
static int sys_getcwd(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_getcwd,		"getcwd"	},  /* 79 */
static int sys_chdir(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TF,	sys_chdir,		"chdir"		},  /* 80 */
static int sys_fchdir(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_fchdir,		"fchdir"	},  /* 81 */
static int sys_rename(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_rename,		"rename"	},  /* 82 */
static int sys_mkdir(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_mkdir,		"mkdir"		},  /* 83 */
static int sys_rmdir(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TF,	sys_rmdir,		"rmdir"		},  /* 84 */
static int sys_creat(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD|TF,	sys_creat,		"creat"		},  /* 85 */
static int sys_link(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_link,		"link"		},  /* 86 */
static int sys_unlink(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TF,	sys_unlink,		"unlink"	},  /* 87 */
static int sys_symlink(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_symlink,		"symlink"	},  /* 88 */
static int sys_readlink(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TF,	sys_readlink,		"readlink"	},  /* 89 */
static int sys_chmod(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_chmod,		"chmod"		},  /* 90 */
static int sys_fchmod(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_fchmod,		"fchmod"	},  /* 91 */
static int sys_chown(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TF,	sys_chown,		"chown"		},  /* 92 */
static int sys_fchown(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_fchown,		"fchown"	},  /* 93 */
static int sys_lchown(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TF,	sys_chown,		"lchown"	},  /* 94 */
static int sys_umask(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_umask,		"umask"		},  /* 95 */
static int sys_gettimeofday(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_gettimeofday,	"gettimeofday"	},  /* 96 */
static int sys_getrlimit(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_getrlimit,		"getrlimit"	},  /* 97 */
static int sys_getrusage(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_getrusage,		"getrusage"	},  /* 98 */
static int sys_sysinfo(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_sysinfo,		"sysinfo"	},  /* 99 */
static int sys_times(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_times,		"times"		},  /* 100 */
static int sys_ptrace(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	0,	sys_ptrace,		"ptrace"	},  /* 101 */
static int sys_getuid(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	NF,	sys_getuid,		"getuid"	},  /* 102 */
static int sys_syslog(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_syslog,		"syslog"	},  /* 103 */
static int sys_getgid(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	NF,	sys_getgid,		"getgid"	},  /* 104 */
static int sys_setuid(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_setuid,		"setuid"	},  /* 105 */
static int sys_setgid(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_setgid,		"setgid"	},  /* 106 */
static int sys_geteuid(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	NF,	sys_geteuid,		"geteuid"	},  /* 107 */
static int sys_getegid(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	NF,	sys_getegid,		"getegid"	},  /* 108 */
static int sys_setpgid(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_setpgid,		"setpgid"	},  /* 109 */
static int sys_getppid(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	0,	sys_getppid,		"getppid"	},  /* 110 */
static int sys_getpgrp(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	0,	sys_getpgrp,		"getpgrp"	},  /* 111 */
static int sys_setsid(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	0,	sys_setsid,		"setsid"	},  /* 112 */
static int sys_setreuid(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_setreuid,		"setreuid"	},  /* 113 */
static int sys_setregid(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_setregid,		"setregid"	},  /* 114 */
static int sys_getgroups(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_getgroups,		"getgroups"	},  /* 115 */
static int sys_setgroups(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_setgroups,		"setgroups"	},  /* 116 */
static int sys_setresuid(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_setresuid,		"setresuid"	},  /* 117 */
static int sys_getresuid(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_getresuid,		"getresuid"	},  /* 118 */
static int sys_setresgid(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_setresgid,		"setresgid"	},  /* 119 */
static int sys_getresgid(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_getresgid,		"getresgid"	},  /* 120 */
static int sys_getpgid(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_getpgid,		"getpgid"	},  /* 121 */
static int sys_setfsuid(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	NF,	sys_setfsuid,		"setfsuid"	},  /* 122 */
static int sys_setfsgid(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	NF,	sys_setfsgid,		"setfsgid"	},  /* 123 */
static int sys_getsid(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_getsid,		"getsid"	},  /* 124 */
static int sys_capget(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_capget,		"capget"	},  /* 125 */
static int sys_capset(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_capset,		"capset"	},  /* 126 */
static int sys_rt_sigpending(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TS,	sys_rt_sigpending,	"rt_sigpending"	},  /* 127 */
static int sys_rt_sigtimedwait(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TS,	sys_rt_sigtimedwait,	"rt_sigtimedwait"	},  /* 128 */
static int sys_rt_sigqueueinfo(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TS,	sys_rt_sigqueueinfo,    "rt_sigqueueinfo"	},  /* 129 */
static int sys_rt_sigsuspend(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TS,	sys_rt_sigsuspend,	"rt_sigsuspend"	},  /* 130 */
static int sys_sigaltstack(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TS,	sys_sigaltstack,	"sigaltstack"	},  /* 131 */
static int sys_utime(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_utime,		"utime"		},  /* 132 */
static int sys_mknod(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TF,	sys_mknod,		"mknod"		},  /* 133 */
static int sys_uselib(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TF,	sys_uselib,		"uselib"	},  /* 134 */
static int sys_personality(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_personality,	"personality"	},  /* 135 */
static int sys_ustat(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_ustat,		"ustat"		},  /* 136 */
static int sys_statfs(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_statfs,		"statfs"	},  /* 137 */
static int sys_fstatfs(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_fstatfs,		"fstatfs"	},  /* 138 */
static int sys_sysfs(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_sysfs,		"sysfs"		},  /* 139 */
static int sys_getpriority(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_getpriority,	"getpriority"	},  /* 140 */
static int sys_setpriority(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_setpriority,	"setpriority"	},  /* 141 */
static int sys_sched_setparam(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	0,	sys_sched_setparam,	"sched_setparam"	},  /* 142 */
static int sys_sched_getparam(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_sched_getparam,	"sched_getparam"	},  /* 143 */
static int sys_sched_setscheduler(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_sched_setscheduler,	"sched_setscheduler"	},  /* 144 */
static int sys_sched_getscheduler(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_sched_getscheduler,	"sched_getscheduler"	},  /* 145 */
static int sys_sched_get_priority_max(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_sched_get_priority_max,	"sched_get_priority_max"	},  /* 146 */
static int sys_sched_get_priority_min(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_sched_get_priority_min,	"sched_get_priority_min"	},  /* 147 */
static int sys_sched_rr_get_interval(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_sched_rr_get_interval,	"sched_rr_get_interval"	},  /* 148 */
static int sys_mlock(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TM,	sys_mlock,		"mlock"		},  /* 149 */
static int sys_munlock(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TM,	sys_munlock,		"munlock"	},  /* 150 */
static int sys_mlockall(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TM,	sys_mlockall,		"mlockall"	},  /* 151 */
static int sys_munlockall(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	TM,	sys_munlockall,		"munlockall"	},  /* 152 */
static int sys_vhangup(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	0,	sys_vhangup,		"vhangup"	},  /* 153 */
static int sys_modify_ldt(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_modify_ldt,		"modify_ldt"	},  /* 154 */
static int sys_pivotroot(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_pivotroot,		"pivot_root"	},  /* 155 */
static int sys_sysctl(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_sysctl,		"_sysctl"	},  /* 156 */
static int sys_prctl(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_prctl,		"prctl"		},  /* 157 */
static int sys_arch_prctl(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TP,	sys_arch_prctl,		"arch_prctl"	},  /* 158 */
static int sys_adjtimex(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_adjtimex,		"adjtimex"	},  /* 159 */
static int sys_setrlimit(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_setrlimit,		"setrlimit"	},  /* 160 */
static int sys_chroot(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TF,	sys_chroot,		"chroot"	},  /* 161 */
static int sys_sync(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	0,	sys_sync,		"sync"		},  /* 162 */
static int sys_acct(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TF,	sys_acct,		"acct"		},  /* 163 */
static int sys_settimeofday(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_settimeofday,	"settimeofday"	},  /* 164 */
static int sys_mount(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TF,	sys_mount,		"mount"		},  /* 165 */
static int sys_umount2(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_umount2,		"umount"	}, /* 166 */
static int sys_swapon(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_swapon,		"swapon"	},  /* 167 */
static int sys_swapoff(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TF,	sys_swapoff,		"swapoff"	},  /* 168 */
static int sys_reboot(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	0,	sys_reboot,		"reboot"	},  /* 169 */
static int sys_sethostname(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_sethostname,	"sethostname"	},  /* 170 */
static int sys_setdomainname(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_setdomainname,	"setdomainname"	},  /* 171 */
static int sys_iopl(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_iopl,		"iopl"		},  /* 172 */
static int sys_ioperm(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_ioperm,		"ioperm"	},  /* 173 */
static int sys_create_module(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_create_module,	"create_module"	},  /* 174 */
static int sys_init_module(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_init_module,	"init_module"	},  /* 175 */
static int sys_delete_module(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_delete_module,	"delete_module"	},  /* 176 */
static int sys_get_kernel_syms(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_get_kernel_syms,	"get_kernel_syms"},  /* 177 */
static int sys_query_module(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_query_module,	"query_module"	},  /* 178 */
static int sys_quotactl(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TF,	sys_quotactl,		"quotactl"	},  /* 179 */
static int sys_nfsservctl(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_nfsservctl,		"nfsservctl"	},  /* 180 */
static int sys_getpmsg(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_getpmsg,		"getpmsg"	}, /* 181 */
static int sys_putpmsg(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_putpmsg,		"putpmsg"	}, /* 182 */
static int sys_afs_syscall(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_afs_syscall,	"afs_syscall"	},  /* 183 */
static int sys_tuxcall(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_tuxcall,		"tuxcall"	}, /* 184 */
static int sys_security(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_security,		"security"	}, /* 185 */
static int sys_gettid(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	0,	sys_gettid,		"gettid"	}, /* 186 */
static int sys_readahead(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD,	sys_readahead,		"readahead"	}, /* 187 */
static int sys_setxattr(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TF,	sys_setxattr,		"setxattr"	}, /* 188 */
static int sys_lsetxattr(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TF,	sys_setxattr,		"lsetxattr"	}, /* 189 */
static int sys_fsetxattr(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD,	sys_fsetxattr,		"fsetxattr"	}, /* 190 */
static int sys_getxattr(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TF,	sys_getxattr,		"getxattr"	}, /* 191 */
static int sys_lgetxattr(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TF,	sys_getxattr,		"lgetxattr"	}, /* 192 */
static int sys_fgetxattr(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD,	sys_fgetxattr,		"fgetxattr"	}, /* 193 */
static int sys_listxattr(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TF,	sys_listxattr,		"listxattr"	}, /* 194 */
static int sys_llistxattr(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TF,	sys_listxattr,		"llistxattr"	}, /* 195 */
static int sys_flistxattr(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_flistxattr,		"flistxattr"	}, /* 196 */
static int sys_removexattr(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_removexattr,	"removexattr"	}, /* 197 */
static int sys_lremovexattr(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_removexattr,	"lremovexattr"	}, /* 198 */
static int sys_fremovexattr(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_fremovexattr,	"fremovexattr"	}, /* 199 */
static int sys_tkill(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TS,	sys_kill,		"tkill"		}, /* 200 */
static int sys_time(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_time,		"time"		},  /* 201 */
static int sys_futex(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	0,	sys_futex,		"futex"		}, /* 202 */
static int sys_sched_setaffinity(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_sched_setaffinity,	"sched_setaffinity" },/* 203 */
static int sys_sched_getaffinity(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_sched_getaffinity,	"sched_getaffinity" },/* 204 */
static int sys_set_thread_area(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_set_thread_area,	"set_thread_area" }, /* 205 */
static int sys_io_setup(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_io_setup,		"io_setup"	}, /* 206 */
static int sys_io_destroy(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_io_destroy,		"io_destroy"	}, /* 207 */
static int sys_io_getevents(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_io_getevents,	"io_getevents"	}, /* 208 */
static int sys_io_submit(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_io_submit,		"io_submit"	}, /* 209 */
static int sys_io_cancel(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_io_cancel,		"io_cancel"	}, /* 210 */
static int sys_get_thread_area(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_get_thread_area,	"get_thread_area" }, /* 211 */
static int sys_lookup_dcookie(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_lookup_dcookie,	"lookup_dcookie"}, /* 212 */
static int sys_epoll_create(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_epoll_create,	"epoll_create"	}, /* 213 */
static int sys_epoll_ctl_old(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	0,	printargs,		"epoll_ctl_old"	}, /* 214 */
static int sys_epoll_wait_old(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	0,	printargs,		"epoll_wait_old"}, /* 215 */
static int sys_remap_file_pages(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TM,	sys_remap_file_pages,	"remap_file_pages"}, /* 216 */
static int sys_getdents64(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_getdents64,		"getdents64"	}, /* 217 */
static int sys_set_tid_address(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_set_tid_address,	"set_tid_address"}, /* 218 */
static int sys_restart_syscall(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	0,	sys_restart_syscall,	"restart_syscall"}, /* 219 */
static int sys_semtimedop(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TI,	sys_semtimedop,		"semtimedop"	}, /* 220 */
static int sys_fadvise64_64(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD,	sys_fadvise64_64,	"fadvise64"	}, /* 221 */
static int sys_timer_create(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_timer_create,	"timer_create"	}, /* 222 */
static int sys_timer_settime(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	0,	sys_timer_settime,	"timer_settime"	}, /* 223 */
static int sys_timer_gettime(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_timer_gettime,	"timer_gettime"	}, /* 224 */
static int sys_timer_getoverrun(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_timer_getoverrun,	"timer_getoverrun"}, /* 225 */
static int sys_timer_delete(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_timer_delete,	"timer_delete"	}, /* 226 */
static int sys_clock_settime(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_clock_settime,	"clock_settime"	}, /* 227 */
static int sys_clock_gettime(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_clock_gettime,	"clock_gettime"	}, /* 228 */
static int sys_clock_getres(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_clock_getres,	"clock_getres"	}, /* 229 */
static int sys_clock_nanosleep(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	0,	sys_clock_nanosleep,	"clock_nanosleep"}, /* 230 */
static int sys_exit_group(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TP,	sys_exit,		"exit_group"	}, /* 231 */
static int sys_epoll_wait(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD,	sys_epoll_wait,		"epoll_wait"	}, /* 232 */
static int sys_epoll_ctl(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD,	sys_epoll_ctl,		"epoll_ctl"	}, /* 233 */
static int sys_tgkill(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TS,	sys_tgkill,		"tgkill"	}, /* 234 */
static int sys_utimes(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TF,	sys_utimes,		"utimes"	}, /* 235 */
static int sys_vserver(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_vserver,		"vserver"	}, /* 236 */
static int sys_mbind(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	TM,	sys_mbind,		"mbind"		}, /* 237 */
static int sys_set_mempolicy(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TM,	sys_set_mempolicy,	"set_mempolicy"	}, /* 238 */
static int sys_get_mempolicy(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TM,	sys_get_mempolicy,	"get_mempolicy"	}, /* 239 */
static int sys_mq_open(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	0,	sys_mq_open,		"mq_open"	}, /* 240 */
static int sys_mq_unlink(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	0,	sys_mq_unlink,		"mq_unlink"	}, /* 241 */
static int sys_mq_timedsend(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_mq_timedsend,	"mq_timedsend"	}, /* 242 */
static int sys_mq_timedreceive(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_mq_timedreceive,	"mq_timedreceive" }, /* 243 */
static int sys_mq_notify(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_mq_notify,		"mq_notify"	}, /* 244 */
static int sys_mq_getsetattr(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_mq_getsetattr,	"mq_getsetattr"	}, /* 245 */
static int sys_kexec_load(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	0,	sys_kexec_load,		"kexec_load"	}, /* 246 */
static int sys_waitid(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TP,	sys_waitid,		"waitid"	}, /* 247 */
static int sys_add_key(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_add_key,		"add_key"	}, /* 248 */
static int sys_request_key(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	0,	sys_request_key,	"request_key"	}, /* 249 */
static int sys_keyctl(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	0,	sys_keyctl,		"keyctl"	}, /* 250 */
static int sys_ioprio_set(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_ioprio_set,		"ioprio_set"	}, /* 251 */
static int sys_ioprio_get(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_ioprio_get,		"ioprio_get"	}, /* 252 */
static int sys_inotify_init(struct tcb *tcp) { return sys_generic(tcp); } // { 0,	TD,	sys_inotify_init,	"inotify_init"	}, /* 253 */
static int sys_inotify_add_watch(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_inotify_add_watch,	"inotify_add_watch" }, /* 254 */
static int sys_inotify_rm_watch(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_inotify_rm_watch,	"inotify_rm_watch" }, /* 255 */
static int sys_migrate_pages(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TM,	sys_migrate_pages,	"migrate_pages"	}, /* 256 */
static int sys_openat(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD|TF,	sys_openat,		"openat"	}, /* 257 */
static int sys_mkdirat(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD|TF,	sys_mkdirat,		"mkdirat"	}, /* 258 */
static int sys_mknodat(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD|TF,	sys_mknodat,		"mknodat"	}, /* 259 */
static int sys_fchownat(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD|TF,	sys_fchownat,		"fchownat"	}, /* 260 */
static int sys_futimesat(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD|TF,	sys_futimesat,		"futimesat"	}, /* 261 */
static int sys_newfstatat(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD|TF,	sys_newfstatat,		"newfstatat"	}, /* 262 */
static int sys_unlinkat(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD|TF,	sys_unlinkat,		"unlinkat"	}, /* 263 */
static int sys_renameat(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD|TF,	sys_renameat,		"renameat"	}, /* 264 */
static int sys_linkat(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD|TF,	sys_linkat,		"linkat"	}, /* 265 */
static int sys_symlinkat(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD|TF,	sys_symlinkat,		"symlinkat"	}, /* 266 */
static int sys_readlinkat(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD|TF,	sys_readlinkat,		"readlinkat"	}, /* 267 */
static int sys_fchmodat(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD|TF,	sys_fchmodat,		"fchmodat"	}, /* 268 */
static int sys_faccessat(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD|TF,	sys_faccessat,		"faccessat"	}, /* 269 */
static int sys_pselect6(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	TD,	sys_pselect6,		"pselect6"	}, /* 270 */
static int sys_ppoll(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD,	sys_ppoll,		"ppoll"		}, /* 271 */
static int sys_unshare(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TP,	sys_unshare,		"unshare"	}, /* 272 */
static int sys_set_robust_list(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_set_robust_list,	"set_robust_list" }, /* 273 */
static int sys_get_robust_list(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_get_robust_list,	"get_robust_list" }, /* 274 */
static int sys_splice(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	TD,	sys_splice,		"splice"	}, /* 275 */
static int sys_tee(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD,	sys_tee,		"tee"		}, /* 276 */
static int sys_sync_file_range(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD,	sys_sync_file_range,	"sync_file_range" }, /* 277 */
static int sys_vmsplice(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD,	sys_vmsplice,		"vmsplice"	}, /* 278 */
static int sys_move_pages(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	TM,	sys_move_pages,		"move_pages"	}, /* 279 */
static int sys_utimensat(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD|TF,	sys_utimensat,		"utimensat"	}, /* 280 */
static int sys_epoll_pwait(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	TD,	sys_epoll_pwait,	"epoll_pwait"	}, /* 281 */
static int sys_signalfd(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD|TS,	sys_signalfd,		"signalfd"	}, /* 282 */
static int sys_timerfd_create(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_timerfd_create,	"timerfd_create"}, /* 283 */
static int sys_eventfd(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_eventfd,		"eventfd"	}, /* 284 */
static int sys_fallocate(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	TD,	sys_fallocate,		"fallocate"	}, /* 285 */
static int sys_timerfd_settime(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD,	sys_timerfd_settime,	"timerfd_settime"}, /* 286 */
static int sys_timerfd_gettime(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_timerfd_gettime,	"timerfd_gettime"}, /* 287 */
static int sys_accept4(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TN,	sys_accept4,		"accept4"	}, /* 288 */
static int sys_signalfd4(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TD|TS,	sys_signalfd4,		"signalfd4"	}, /* 289 */
static int sys_eventfd2(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_eventfd2,		"eventfd2"	}, /* 290 */
static int sys_epoll_create1(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_epoll_create1,	"epoll_create1"	}, /* 291 */
static int sys_dup3(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_dup3,		"dup3"		}, /* 292 */
static int sys_pipe2(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_pipe2,		"pipe2"		}, /* 293 */
static int sys_inotify_init1(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_inotify_init1,	"inotify_init1"	}, /* 294 */
static int sys_preadv(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD,	sys_preadv,		"preadv"	}, /* 295 */
static int sys_pwritev(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD,	sys_pwritev,		"pwritev"	}, /* 296 */
static int sys_rt_tgsigqueueinfo(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TP|TS,	sys_rt_tgsigqueueinfo,	"rt_tgsigqueueinfo"}, /* 297 */
static int sys_perf_event_open(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD,	sys_perf_event_open,	"perf_event_open"}, /* 298 */
static int sys_recvmmsg(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TN,	sys_recvmmsg,		"recvmmsg"	}, /* 299 */
static int sys_fanotify_init(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_fanotify_init,	"fanotify_init"	}, /* 300 */
static int sys_fanotify_mark(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD|TF,	sys_fanotify_mark,	"fanotify_mark"	}, /* 301 */
static int sys_prlimit64(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	0,	sys_prlimit64,		"prlimit64"	}, /* 302 */
static int sys_name_to_handle_at(struct tcb *tcp) { return sys_generic(tcp); } // { 5,	TD|TF,	sys_name_to_handle_at,	"name_to_handle_at"}, /* 303 */
static int sys_open_by_handle_at(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	TD,	sys_open_by_handle_at,	"open_by_handle_at"}, /* 304 */
static int sys_clock_adjtime(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	0,	sys_clock_adjtime,	"clock_adjtime"	}, /* 305 */
static int sys_syncfs(struct tcb *tcp) { return sys_generic(tcp); } // { 1,	TD,	sys_syncfs,		"syncfs"	}, /* 306 */
static int sys_sendmmsg(struct tcb *tcp) { return sys_generic(tcp); } // { 4,	TN,	sys_sendmmsg,		"sendmmsg"	}, /* 307 */
static int sys_setns(struct tcb *tcp) { return sys_generic(tcp); } // { 2,	TD,	sys_setns,		"setns"		}, /* 308 */
static int sys_getcpu(struct tcb *tcp) { return sys_generic(tcp); } // { 3,	0,	sys_getcpu,		"getcpu"	}, /* 309 */
static int sys_process_vm_readv(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	0,	sys_process_vm_readv,	"process_vm_readv"	}, /* 310 */
static int sys_process_vm_writev(struct tcb *tcp) { return sys_generic(tcp); } // { 6,	0,	sys_process_vm_writev,	"process_vm_writev"	}, /* 311 */


const struct_sysent sysent0[] = {
#include "syscallent_mod.h"
};

using std::cout;
using std::endl;
using std::hex;
using std::dec;

/*! System Call.
 */
class SysCall {
public:
    SysCall(uint16_t nr): m_nr(nr) {}
private:
    // int	(*m_sys_func)(struct tcb *tcp) = nullptr; ///< The function that was called.
    uint16_t m_nr:15; ///< System call number.
};

int trace2(int argc, const char * argv[], const char * envp[])
{
    int status;
    int pid = fork();
    if (pid == 0) {                      // if this is the forked process
        execl("/bin/ls", "ls", nullptr); // replace forked process with call
        printf("execl failed...\n"); // this statement should never be reached
    } else {                    // parent process
        sleep(1);
        kill(pid, SIGINT);
        wait(&status); // wait for child to stop
        if (WIFSTOPPED(status))
            printf("child has stopped...\n");
        sleep(3);
        ptrace(PTRACE_CONT, pid, 0, 0);
        /* Will not come out of this wait because child does not terminate. */
        wait(&status);
    }
    return 0;
}

int trace4(int argc, const char * argv[], const char * envp[])
{
    int pid = fork();
    bool traceme = true;
    if (pid == 0) {                      // if this is the forked process
        if (traceme) {
            ptrace(PTRACE_TRACEME, 0, 0, 0); // this forked process wants to be traced
        }
        // int c = 0;
        // while (true) {
        //     cout << "sleeping " << c++ << endl;
        //     sleep(1);
        // }
        execl("/bin/ls", "ls", nullptr); // replace forked process with call
        printf("execl failed...\n"); // this statement should never be reached
    } else {                         // parent process
        int status;
        wait(&status);
        if (WIFSTOPPED(status)) {
            printf("child has stopped...\n");
        }
        sleep(1);
        ptrace(PTRACE_CONT, pid, 0, 0);
        wait(&status);
    }
    return 0;
}

/*! Show Process Environment Pointer at \p envp. */
void show_env(const char * envp[])
{
    auto envi = envp;
    while (*envi)
        cout << *(envi++) << endl;
}

void getdata(pid_t pid, long addr, char *str, int len)
{
    char *laddr;
    int i, j;
    union u {
        long val;
        char chars[WORDSIZE];
    } data;
    i = 0;
    j = len / WORDSIZE;
    laddr = str;
    while(i < j) {
        data.val = ptrace(PTRACE_PEEKDATA,
                          pid, addr + i * WORDSIZE,
                          NULL);
        memcpy(laddr, data.chars, WORDSIZE);
        ++i;
        laddr += WORDSIZE;
    }
    j = len % WORDSIZE;
    if(j != 0) {
        data.val = ptrace(PTRACE_PEEKDATA,
                          pid, addr + i * WORDSIZE,
                          NULL);
        memcpy(laddr, data.chars, j);
    }
    str[len] = '\0';
}

void putdata(pid_t pid, long addr, char *str, int len)
{
    char *laddr;
    int i, j;
    union u {
        long val;
        char chars[WORDSIZE];
    }data;
    i = 0;
    j = len / WORDSIZE;
    laddr = str;
    while(i < j) {
        memcpy(data.chars, laddr, WORDSIZE);
        ptrace(PTRACE_POKEDATA, pid,
               addr + i * WORDSIZE, data.val);
        ++i;
        laddr += WORDSIZE;
    }
    j = len % WORDSIZE;
    if(j != 0) {
        memcpy(data.chars, laddr, j);
        ptrace(PTRACE_POKEDATA, pid,
               addr + i * WORDSIZE, data.val);
    }
}

int test_strace(int argc, const char * argv[], const char * envp[])
{
    int pid = fork();
    if (pid == 0) {                      // if this is the forked process
        cout << "child: pid:" << getpid() << " ppid:" << getppid();
        // cout << "Environment:" << endl; show_env(envp);
        ptrace(PTRACE_TRACEME, 0, 0, 0);
        kill(getpid(), SIGINT); // stop child so parent can do ptrace(PTRACE_SYSCALL);
        execle("/bin/ls", "ls", nullptr, envp); // replace forked process with call
    } else {                    // parent process
        cout << "parent: pid:" << getpid() << " ppid:" << getppid();

        int status;

        wait(&status);          // for state changes in a child of the calling process

        while (true) {
            // trigger syscall of child
            ptrace(PTRACE_SYSCALL, pid, 0, 0); // use PTRACE_SYSCALL or PTRACE_CONT for normal progress
            wait(&status);

            cout << "parent: status:" << status << " means ";

            if (WIFEXITED(status)) {
                printf("parent: child exited\n");
                break;
            } else if (WIFSTOPPED(status)) {
                printf("parent: child stopped\n");
            }

            struct user_regs_struct ur;
            ptrace(PTRACE_GETREGS, pid, 0, &ur); // get registers
            // show_n(reinterpret_cast<uchar*>(&ur), sizeof(ur));

            const auto nr = ur.orig_ax; // system call code/number

            // true if system call is entering kernel, false if exiting/returning
            const bool entering = (ur.ax bitand 0xff) == 0xda;

            if (true) { // open()
                // const auto orig_ax = ptrace(PTRACE_PEEKUSER, pid, WORDSIZE * ORIG_EAX, nullptr);

                cout << "fn:" << sysent0[nr].sys_name;
                cout << " nr:" << nr;

                if (entering) { // entering
                } else {        // exiting
                    long ret = ptrace(PTRACE_PEEKUSER, pid, WORDSIZE*ur.ax, nullptr);
                    cout << " => " << ret;
                }

                if (sysent0[nr].sys_func == sys_open) { // int open(const char *pathname, int flags);
                    long params[] = { ptrace(PTRACE_PEEKUSER, pid, WORDSIZE*EBX, nullptr),
                                      ptrace(PTRACE_PEEKUSER, pid, WORDSIZE*ECX, nullptr) };
                    if (entering) {
                        cout << " pathname:0x" << hex << params[0] << dec;
                        cout << " flags:0x" << hex << params[1] << dec;
                    }
                } else if (sysent0[nr].sys_func == sys_write) { // ssize_t write(int fd, const void *buf, size_t count)
                    long params[] = { ptrace(PTRACE_PEEKUSER, pid, WORDSIZE*EBX, nullptr),
                                      ptrace(PTRACE_PEEKUSER, pid, WORDSIZE*ECX, nullptr),
                                      ptrace(PTRACE_PEEKUSER, pid, WORDSIZE*EDX, nullptr)};
                    if (entering) {
                        char str[params[2]+1]; // len + terminator
                        getdata(pid, params[1], str, params[2]);
                        cout << " fd:" << params[0];
                        cout << " buf:0x" << hex << params[1] << dec << " str:" << str;
                        cout << " len:" << params[2];
                        // free(str);
                    }
                }

                auto ins = ptrace(PTRACE_PEEKTEXT, pid, ur.ip, nullptr);
                printf(" EIP: %lx Instruction executed: %lx\n", ur.ip, ins);
            }
            if (false) {
                cout << "parent: name=" << sysent0[nr].sys_name
                     << " nr=" << nr
                     << " r11=" << hex << ur.r11 << dec
                     << " r10=" << hex << ur.r10 << dec
                     << " r9=" << hex << ur.r9 << dec
                     << " r8=" << hex << ur.r8 << dec
                     << " ax=" << hex << ur.ax << dec
                     << " ax8=" << hex << (ur.ax bitand 0xff) << dec
                     << " flags=" << ur.flags << endl;
            }

            if (false) {
                ur.orig_ax = 64; // modify system call
                ptrace(PTRACE_SETREGS, pid, 0, &ur);
            }

            if (not entering) cout << endl;
        }
    }
    return 0;
}

// argv[1] should be name of an object file

/*! Test BFD.
  * \see http://pramode.net/articles/lfy/ptrace/pramode.html
  */
void test_bfd(int argc, char *argv[])
{
    long storage_needed, num_symbols, i;
    asymbol **symbol_table;
    bfd *abfd;
    // char filename[100];

    bfd_init(); // magic

    abfd = bfd_openr(argv[1], nullptr);
    assert(abfd != nullptr);
    bfd_check_format(abfd, bfd_object);
    storage_needed = bfd_get_symtab_upper_bound(abfd);
    assert(storage_needed >= 0);
    printf("storage = %ld\n", storage_needed);

    symbol_table = (asymbol**)malloc(storage_needed);
    assert(symbol_table != 0);
    num_symbols = bfd_canonicalize_symtab(abfd, symbol_table);
    assert(num_symbols >= 0);
    printf("num syms = %ld\n", num_symbols);
    for(i = 0; i < num_symbols; i++)
        printf("%s: %lx\n", bfd_asymbol_name(symbol_table[i]),
               bfd_asymbol_value(symbol_table[i]));

}

int main(int argc, const char * argv[], const char * envp[])
{
    return test_strace(argc, argv, envp);
}
