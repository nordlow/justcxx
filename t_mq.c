#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <mqueue.h>
#include <sys/stat.h>		/* S_IRWXU */

/*!
 * Test UNIX Message Queues.
 */
int
main(int argc, char *argv[])
{
  struct mq_attr mq_attr;
  mq_attr.mq_maxmsg = 100;
  mq_attr.mq_msgsize = 128;
  mq_attr.mq_flags = 0;
  mq_attr.mq_curmsgs = 0;

  mqd_t mq;
  mq = mq_open("/message", O_CREAT | O_RDWR,	/* <sys/fcntl.h> */
	       S_IRWXU,		/* <sys/stat.h> */
	       &mq_attr);

  int ret;
  const char msg[] = "testmessage";
  uint prio = 0;
  ret = mq_send(mq, msg, sizeof(msg), prio);

  char *rcv_msg;
  rcv_msg = (char*)(malloc(mq_attr.mq_msgsize));

  uint rcv_prio;
  ret = mq_receive(mq, rcv_msg, mq_attr.mq_msgsize, &rcv_prio);

  printf("%s\n", rcv_msg);
  printf("prio: %d\n", rcv_prio);

  free(rcv_msg);

  if (mq == (mqd_t) - 1) {
    perror("mq_open");
    return -1;
  }

  mq_close(mq);

  return 0;
}
