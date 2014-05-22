#undef LWIP_UDP
/* #define LWIP_UDP (1) */

#ifndef MAX
#define MAX2(a,b) (((a) > (b)) ? (a) : (b))
#endif

#if LWIP_UDP
#include "xparameters.h"
#endif

#include <time.h>
#include <sys/types.h>

#if LWIP_UDP
#include <lwip/ip_addr.h>
#include "lwipopts.h"
#include "lwip/sys.h"
#include "lwip/sockets.h"
#include <lwip/inet.h>
#else
#include <sys/socket.h>
#include <resolv.h>		/* DNS lookups */
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

#include "fdutils.h"

int
udp_socket(int port)
{
  int fd;

  /* structures for representation of internet addresses */
  struct sockaddr_in saddr;
  int saddr_sz;

  /* create socket(file descriptor) */
  if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    return -1;
  }

  /* set up the address */
  saddr_sz = sizeof(saddr);
  memset((char *) &saddr, 0, saddr_sz);
  saddr.sin_family = AF_INET;  /* address family internet */

  /* kind of a mask. choices are INADDR_LOOPBACK and INADDR_ANY */
  saddr.sin_addr.s_addr = htonl(INADDR_ANY);
  saddr.sin_port = htons(port);

  /* and bind it to our local socket we want to talk on */
  if (bind(fd, (struct sockaddr *) &saddr, saddr_sz) < 0) {
    perror("bind");
    return -1;
  }

  unblockfd(fd);

  return fd;
}

/*!
 * Receive buffer buf of length len bytes through fd.
 *
 * Stores sender in saddr.
 */
ssize_t
udp_recvbuf(int fd, char *buf, size_t len, struct sockaddr_in *saddr)
{
  socklen_t saddr_sz = sizeof(*saddr);
  return recvfrom(fd, buf, len, 0, (struct sockaddr *)saddr, &saddr_sz);
}

/*!
 * Send buffer buf of length len bytes through fd to destination saddr.
 */
ssize_t
udp_sendbuf(int fd, const char *buf, size_t len, struct sockaddr_in *saddr)
{
  int saddr_sz = sizeof(*saddr);
  return sendto(fd, (char*)buf, len, 0, (struct sockaddr *)saddr, saddr_sz);
}

/*!
 * Initialize the socket address structure saddr with ip_string:port.
 *
 * \param[in] ip_string IP-address given in as a byte-quad (for example 127.0.0.1).
 * \param[in] port socket port number
 */
int
inet_initaddr(struct sockaddr_in *saddr, const char *ip_string, int port)
{
  struct in_addr in_addr;
  if (inet_aton(ip_string, &in_addr)) {	/* from "x.x.x.x" to binary */
    int saddr_sz = sizeof(*saddr);
    memset((char *) saddr, 0, saddr_sz);
    saddr->sin_family = AF_INET;	/* address family internet */
    saddr->sin_addr.s_addr = in_addr.s_addr;
    saddr->sin_port = htons(port);
    return 0;
  } else {
    fprintf(stderr, "%s: invalid address format: %s\n", __FUNCTION__,
	    ip_string);
    return -1;
  }
}

#define HUP_UDP_PORT (5000)	/**< Default UDP port. */

void
chararray_print_humanely(const char *buf, size_t len)
{
  size_t i;
  for (i = 0; i < len; i++) {
    const int ch = buf[i];
#ifdef VXWORKS
    if ((ch >= 'a' && ch <= 'z') ||
	(ch >= '0' && ch <= '9')) {
      printf("'%c' ", ch);
    }
#else
    if (isgraph(ch) && !isspace(ch)) {
      printf("'%c' ", ch);
    }
#endif
    else {
      printf("0x%02x ", (unsigned char)ch);
    }
  }
}

#define MSG_STD ('S')		/**< Standard Message */
#define MSG_ACK ('A')		/**< Acknowledgement Message */

int
main(int argc, char *argv[])
{
  int g_datfd = -1, g_stdin = -1, ret = -1, max_fd = -1;
  fd_set readset, writeset;
  int local_port = 0, remote_port = 0;
  const char * remote_host = NULL;
  int outbox_cnt = 0;		/* outbox message counter */
  int inbox_cnt = 0;		/* inbox message counter */
  int outack_cnt = 0;		/* outack message counter */
  int inack_cnt = 0;		/* inack message counter */

  if (argc == 1+0) {
    printf("Defaulting local_port = %d\n", local_port = HUP_UDP_PORT);
    printf("Defaulting remote_port = %d\n", remote_port = HUP_UDP_PORT);
  } else if (argc == 1+1) {
    printf("Using local_port = %d\n", local_port = atoi(argv[1]));
    printf("Defaulting remote_port = %d\n", remote_port = HUP_UDP_PORT);
  } else if (argc == 1+2) {
    printf("Using local_port = %d\n", local_port = atoi(argv[1]));
    printf("Using remote_host = \"%s\"\n", remote_host = argv[2]);
    printf("Defaulting remote_port = %d\n", remote_port = HUP_UDP_PORT);
  } else if (argc == 1+3) {
    printf("Using local_port = %d\n", local_port = atoi(argv[1]));
    printf("Using remote_host = \"%s\"\n", remote_host = argv[2]);
    printf("Using remote_port = %d\n", remote_port = atoi(argv[3]));
  } else {
    fprintf(stderr, "Usage: %s [LOCAL_PORT] [REMOTE_HOST] [REMOTE_PORT]\n",
	    argv[0]);
    return -1;
  }

  /* open socket */
  if ((g_datfd = udp_socket(local_port)) < 0) {
    fprintf(stderr, "Could not open UDP-socket for HUP-data: \n");
    perror("");
    g_datfd = -1;
  }

  struct sockaddr_in raddr;	/* remote address */
  if (remote_host) {
    inet_initaddr(&raddr, remote_host, remote_port);
  }

  g_stdin = fileno(stdin);

  while (1) {			/* mainloop */
    FD_ZERO(&readset);
    FD_ZERO(&writeset);

    FD_SET(g_stdin, &readset); max_fd = MAX2(max_fd, g_stdin);
    FD_SET(g_datfd, &readset); max_fd = MAX2(max_fd, g_datfd);

    ret = select(max_fd + 1, &readset, NULL, NULL, NULL); /* wait event */
    if (ret == -1) {
      perror("select");
      return -1;
    }

    if (g_datfd != -1 &&		/* if connection is opened */
	FD_ISSET(g_datfd, &readset)) { /* and their are pending data on it */
      char pbuf[65536];		/* packet buffer */
      struct sockaddr_in saddr;
      ssize_t psize = udp_recvbuf(g_datfd, pbuf, sizeof(pbuf), &saddr);
      if (psize < 0) {
	printf("error when reading HUP-data\n");
      } else if (psize == 0) {
	close(g_datfd);
	g_datfd = -1;
	printf("read from g_datfd returned 0!.\n");
      } else {
	printf("recv [ '%c' ", pbuf[0]);
	char * mbuf = pbuf+1;	/* message buffer */
	ssize_t msize = psize-1; /* message size */
	if (msize > 0) {
	  chararray_print_humanely(mbuf, msize);
	}

	if (pbuf[0] != MSG_ACK) { /* if not an acknowledgement */
	  printf("] from %s:%d (inbox nr %d)\n",
		 inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port),
		 inbox_cnt++);
	} else {
	  printf("] from %s:%d (inack nr %d)\n",
		 inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port),
		 inack_cnt++);
	}

	if (pbuf[0] != MSG_ACK) { /* if not an acknowledgement */
	  pbuf[0] = MSG_ACK;	/* change header to acknowledgement */
	  udp_sendbuf(g_datfd, pbuf, psize, &saddr);

	  printf("send [ ");
	  chararray_print_humanely(pbuf, psize);
	  printf("] to %s:%d (outack nr %d)\n",
		 inet_ntoa(saddr.sin_addr), ntohs(saddr.sin_port),
		 outack_cnt++);
	}
      }
    }

    if (FD_ISSET(g_stdin, &readset)) { /* if user input */
      char linebuf[80];
      linebuf[0] = MSG_STD;
      char * mbuf = linebuf+1;	/* message buffer */
      fgets(mbuf, sizeof(linebuf) - 1, stdin);
      linebuf[79] = 0;		/* assure null termination */

      /* if ends with \n replace it with \0 */
      if (linebuf[strlen(linebuf) - 1] == '\n') {
	linebuf[strlen(linebuf) - 1] = '\0';
      }

      if (remote_host) {
	udp_sendbuf(g_datfd, linebuf, strlen(linebuf), &raddr);

	printf("send [ ");
	chararray_print_humanely(linebuf, strlen(linebuf));
	printf("] to %s:%d (outbox nr %d)\n",
	       inet_ntoa(raddr.sin_addr), ntohs(raddr.sin_port),
	       outbox_cnt++);
      } else {
	fprintf(stderr, "Cannot send message because no remote_host was given at commandline\n");
      }

      /* quit */
      if (strcmp(mbuf, "q") == 0 ||
	  strcmp(mbuf, "quit") == 0) {
	printf("quitting...\n");
	break;
      }
    }
  }

  close(g_datfd);

  return 0;
}
