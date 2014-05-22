#include "sockutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>		/* Defines in_addr_t which is needed in <netinet/in.h>  */
#include <netinet/in.h>
#include <arpa/inet.h>

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * \file sockutils.c
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * The following structure holds socket address information for many types of
 * sockets:

 * struct sockaddr {
 *    unsigned short    sa_family;    // address family, AF_xxx
 *    char              sa_data[14];  // 14 bytes of protocol address
 * };

 * sa_family can be a variety of things, but it'll be AF_INET for everything
 * we do for now. sa_data contains a destination address and port number for
 * the socket. This is rather unwieldy since you don't want to
 * tediously pack the address in the sa_data by hand.

 * To deal with struct sockaddr, programmers created a parallel structure:
 * struct sockaddr_in ("in" for "Internet".)

 * struct sockaddr_in {
 *    short int          sin_family;  // Address family
 *    unsigned short int sin_port;    // Port number
 *    struct in_addr     sin_addr;    // Internet address
 *    uchar      sin_zero[8]; // Same size as struct sockaddr
 * };

 * This structure makes it easy to reference elements of the socket address.
 * Note that sin_zero (which is included to pad the structure to the length of
 * a struct sockaddr) should be set to all zeros with the function memset ().
 * Also, and this is the important bit, a pointer to a struct sockaddr_in can
 * be cast to a pointer to a struct sockaddr and vice-versa. So even though
 * socket () wants a struct sockaddr*, you can still use a struct sockaddr_in
 * and cast it at the last minute! Also, notice that sin_family corresponds to
 * sa_family in a struct sockaddr and should be set to "AF_INET". Finally, the
 * sin_port and sin_addr must be in Network Byte Order!

 * "But," you object, "how can the entire structure, struct in_addr sin_addr,
 * be in Network Byte Order?" This question requires careful examination of
 * the structure struct in_addr, one of the worst unions alive:

 * // Internet address (a structure for historical reasons)
 * struct in_addr {
 *     ulong s_addr; // that's a 32-bit long, or 4 bytes
 * };

 * Well, it used to be a union, but now those days seem to be gone. Good
 * riddance. So if you have declared ina to be of type struct sockaddr_in,
 * then ina.sin_addr.s_addr references the 4-byte IP address (in Network Byte
 * Order). Note that even if your system still uses the God-awful union for
 * struct in_addr, you can still reference the 4-byte IP address in exactly
 * the same way as I did above (this due to #defines.)
 */

/* ========================================================================= */

int
atoport(const char *service, const char *protocol)
{
  int port;
  long lport;
  struct servent *serv;
  char *errpos;

  /* First try to read it from /etc/services */
  serv = getservbyname(service, protocol);
  if (serv != NULL)
    port = serv->s_port;
  else {			/* Not in services, maybe a number? */
    lport = strtol(service, &errpos, 0);
    if (errpos[0] != 0 || lport < 1 || lport > 65535)
      return -1;		/* Invalid port address */
    port = hton16(lport);
  }

  return port;
}

struct in_addr *
atoaddr(char *address)
{
  static struct in_addr inaddr;
  struct hostent *hostent;

  /* First: try it as aaa.bbb.ccc.ddd. */
  inaddr.s_addr = inet_addr(address);	/* Returns address in Network Order */
  if (inaddr.s_addr != 0xffffffffL)     /* -1 (255.255.255.255) means error. */
    return &inaddr;

  /* Second: try a hostname lookup. */
  hostent = gethostbyname(address);
  if (hostent != NULL)
    return (struct in_addr *) *hostent->h_addr_list;

  /* If that does not work either, return NULL to indicate error. */
  return NULL;
}

/* ---------------------------- Group Separator ---------------------------- */

int
makeListenerTCP(int port, int queue_max_length)
{
  int fd, opt;
  struct sockaddr_in address;
  int domain = AF_INET, type = SOCK_STREAM, protocol = 0;

  /* create socket */
  fd = socket(domain, type, protocol);
  if (fd < 0) {
    lperror("socket");
    return -1;
  }

  /* set socket options */
  opt = 1;
  /* Tillåt användande av porten även om den hänger i CLOSE_WAIT. Denna
     option skall alltid användas. */
  if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
		 (char *) &opt, sizeof(opt)) == -1) {
    lperror("setsockopt");
    /* Not a critical error so do not return error.  */
  }

  memset(&address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = hton32(INADDR_ANY);
  address.sin_port = hton16(port);

  /* bind it to a port */
  if (bind(fd, (struct sockaddr *) &address, sizeof(address)) == -1) {
    lperror("bind");
    close(fd);
    return -1;
  }

  /* listen for connections */
  if (listen(fd, queue_max_length) == -1) {
    lperror("listen");
    close(fd);
    return -1;
  }

  return fd;
}

int
getConnectionTCP(int fd)
{
  int c_fd = -1;
  struct sockaddr_in address;
  uint address_len = sizeof(address);

  while (c_fd < 0) {
    c_fd = accept(fd, (struct sockaddr *) &address, &address_len);
    /* Either a real error occured, or blocking was interrupted for
     * some reason. Only abort execution if a real error occured. */
    if (c_fd < 0) {
      if (errno == EINTR) {
	continue;		/* If interrupted try again. */
      } else {
	lperror("accept");
	close(fd);
	return -1;
      }
    }
  }

  lprintf("got connection from %s\n",
	 inet_ntoa(*(struct in_addr *) &address.sin_addr.s_addr));

  return c_fd;
}

/* ---------------------------- Group Separator ---------------------------- */

int
establish(int type, const char *host, const char *service)
{
  /* First convert service from a string, to a number... */
  int port = -1;
  struct in_addr *addr;
  int fd;
  struct sockaddr_in address;

  switch (type) {
  case SOCK_STREAM: port = atoport(service, "tcp"); break;
  case SOCK_DGRAM: port = atoport(service, "udp"); break;
  default: port = -1; break;
  }

  if (port == -1) {
    leprintf("invalid socket type: %d.\n", type);
    return -1;
  }

  addr = atoaddr((char*)host);
  if (addr == NULL) {
    leprintf("invalid network address.\n");
    return -1;
  }
  memset((char *) &address, 0, sizeof(address));
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = addr->s_addr;
  address.sin_port = hton16(port);

  fd = socket(AF_INET, type, 0);

  lprintf("connecting to %s on port %d.\n", inet_ntoa(*addr), hton16(port));

  switch (type) {
  case SOCK_STREAM:
    if (connect(fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
      lperror("connect");
      return -1;
    }
    break;
  case SOCK_DGRAM:
    if (bind(fd, (struct sockaddr *) &address, sizeof(address)) < 0) {
      lperror("bind");
      return -1;
    }
    break;
  default:
    fd = -1;
    break;
  }

  return fd;
}

int
establishTCP(const char *host, const char *service)
{
  return establish(SOCK_STREAM, host, service);
}

int
establishUDP(const char *host, const char *service)
{
  return establish(SOCK_DGRAM, host, service);
}

/* ---------------------------- Group Separator ---------------------------- */

ssize_t
tough_write(int fd, const char *buf, size_t count)
{
  size_t c = 0;
  ssize_t ret;
  while (c < count) {
    do {
      ret = write(fd, buf, count - c);
    } while (ret < 0 && errno == EINTR);	/* If interrupted try again. */
    if (ret <= 0)
      return ret;
    c += ret;
    buf += ret;
  }
  return count;
}

int
tough_array_write(int fd, const char *buf, size_t count)
{
  ssize_t ret;
  uint32_t net_len = hton32(count);
  ret = tough_write(fd, (char *) &count, sizeof(net_len));
  if (ret < 0)
    return ret;
  return tough_write(fd, buf, count);
}

ssize_t
tough_writestr(int fd, const char *str)
{
  return tough_write(fd, str, strlen(str) + 1);	/* Plus one for trailing '\0' */
}

/* ---------------------------- Group Separator ---------------------------- */

int
tough_read(int fd, char *buf, int maxlen)
{
  int c = 0;
  int ret;
  while (c < maxlen) {
    do {
      ret = read(fd, buf, maxlen - c);
    } while (ret < 0 && errno == EINTR); /* If interrupted try again. */
    if (ret < 0)                         /* Error */
      return ret;
    else if (ret == 0)		/* Connection was lost. */
      return c;			/* \TODO: Return c or ret? */
    c += ret;
    buf += ret;
  }
  return maxlen;
}

int
tough_readstr(int fd, char *str, int maxstrlen)
{
  int c = 0;
  int ret;
  while (c < maxstrlen + 1) {	/* One extra for trailing '\0' */
    do {
      ret = read(fd, str, 1);	/* Read one char only. */
    } while (ret < 0 && errno == EINTR);	/* If interrupted try again. */
    if (ret < 0) {		/* Error */
      *str = 0;			/* Plus trailing '\0' */
      return ret;
    } else if (ret == 0) {	/* Connection was lost. */
      *str = 0;			/* Plus trailing '\0' */
      return c;			/* \TODO: Return c or ret? */
    } else if (*str == 0) {
      return c;
    }
    c++;
    str++;
  }
  *str = 0;			/* Plus trailing '\0' */
  return maxstrlen;
}

/* ---------------------------- Group Separator ---------------------------- */

int
select1rw(int fd, bool read_flag, bool write_flag,
	  long to_sec, long to_usec)
{
  fd_set fds;
  fd_set *read_fds, *write_fds, *except_fds;
  struct timeval to;

  to.tv_sec = to_sec;
  to.tv_usec = to_usec;

  FD_ZERO(&fds);
  FD_SET(fd, &fds);

  read_fds = read_flag ? &fds : NULL;
  write_fds = write_flag ? &fds : NULL;

  except_fds = NULL;

  return select(fd + 1, read_fds, write_fds, except_fds, &to);
}

/* ---------------------------- Group Separator ---------------------------- */

void
ignore_pipe(void)
{
/*      struct sigaction sig; */

/*      sig.sa_handler = SIG_IGN; */
/*      sig.sa_flags = 0; */
/*      sigemptyset (&sig.sa_mask); */
/*      sigaction (SIGPIPE, &sig, NULL); */
}

/* ---------------------------- Group Separator ---------------------------- */
