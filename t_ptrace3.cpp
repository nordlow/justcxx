#include <stdio.h>
#include <sys/types.h>
#include <sys/reg.h>
#include <sys/ptrace.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <unistd.h>
#include "helper.h"

int main(int argc, const char * argv[], const char * envp[])
{
    pid_t pid;
    long orig_ax, ax;
    long reg_val[3];
    int status;
    int insyscall =0;
    pid = fork ();
    if (pid == -1)
	printf ("Error in fork");

    if (pid == 0) {
	// In the pid process
	ptrace (PTRACE_TRACEME,0,NULL,NULL);
	execl("/bin/ls", "ls", NULL);
	//execl ("/home/cdac/Documents/ptrace/file_write", "file_write", NULL);
	printf ("In pid process");

    }
    else {
	printf("In parent process\n");
	//In parent process
	while (1){
            int nr;
            struct user_regs_struct u_in, u_out;
            //Loop to catch each of the system calls
            wait (&status);			//wait for the pid process to finish
            if (WIFEXITED(status))		//The pid process exited
                break;
            ptrace(PTRACE_GETREGS,pid, 0, &u_in);
            nr = u_in.orig_ax;
            if (nr == SYS_open) {		// system call is for sys_access
                orig_ax = ptrace (PTRACE_PEEKUSER, pid, 4 * ORIG_AX, NULL);
                if (insyscall == 0){
                    insyscall = 1;
                    reg_val[1] = ptrace (PTRACE_PEEKUSER, pid, 4 * EBX, NULL);
                    reg_val[2] = ptrace (PTRACE_PEEKUSER, pid, 4 * ECX, NULL);
                    reg_val[3] = ptrace (PTRACE_PEEKUSER, pid, 4 * EDX, NULL);		//not used in access syscall
                    printf ("System call is : %d\n", nr);
                    fprintf(stderr, "ebx :%08lx ecx:  %08lx\n ", reg_val[1], reg_val[2]);
                    char *fp = (char *)reg_val[1];						//This is where I was going wrong
                    //During the following line I was getting a segmentationfault
                    printf ("The location of the file is: %s", fp);
                    char buff[10];
                    sprintf(buff, "%lx", reg_val[2]);
                    printf ("the string is %s\n", buff);

                }
                else {
                    ax = ptrace (PTRACE_PEEKUSER, pid, 4 * AX, NULL);
                    printf ("Return value of the SYS_write is %ld\n",ax);
                    insyscall = 0;
                }
            }

            ptrace (PTRACE_SYSCALL, pid, NULL, NULL);
	}
        return 0;
    }
}
