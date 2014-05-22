/*!
 * \file sockutils.h
 * \brief Various utilities related to UNIX socket programming.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include "utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================= */

/*!
 * Take a service name, and a service type, and return a port number.  If the
 * service name is not found, it tries it as a decimal number.  The number
 * returned is byte ordered for the network.
 */
int atoport(const char *service, const char *protocol);

/*!
 * Converts ASCII text in \p address to \c in_addr struct.
 * \return NULL if the address could not be found.
 */
struct in_addr *atoaddr(char *address);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Create a listening TCP-server socket on port \p port.
 * \return the created server socket
 */
int makeListenerTCP(int port, int queue_max_length);

/*!
 * Accept a connection from the listening server socket \p fd.
 * \return new connected socket
 */
int getConnectionTCP(int fd);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Establish a connection to a given server on \p host and \p port.
 *
 * \param[in] type is either \c SOCK_STREAM (TCP) or \c SOCK_DGRAM (UDP),
 * \param[in] host is either xxx.xxx.xxx.xxx or domain name such as www.sunet.se
 * \param[in] service is the port name/number in ASCII form
 *
 * \return new connected socket (>= 0) or -1 if an error occurred.
 */
int establish(int type, const char *host, const char *service);

/*!
 * Create an TCP-connection to a given \p host and \p service.
 */
int establishTCP(const char *host, const char *service);

/*!
 * Create an UDP-connection to a given \p host and \p service.
 */
int establishUDP(const char *host, const char *service);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Like the \c write() system call, accept that it will make sure that
 * all data is transmitted.
 * \return -1 if the connection is closed while it is trying to write.
 */
ssize_t tough_write(int fd, const char *buf, size_t count);

ssize_t tough_array_write(int fd, const char *buf, size_t count);

/*!
 * Write the C-string \p str
 * \return -1 if the connection is closed while it is trying to write.
 */
ssize_t tough_writestr(int fd, const char *str);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Like the \c read() system call, accept that it will make sure that
 * all your data goes through the socket.
 * \return 0 if \p maxlen is 0.
 */
int tough_read(int fd, char *buf, int maxlen);

/*!
 * Like \c tough_read(), but read a C-string at \p str of maximum
 * length \p maxstrlen.
 *
 * If no data is found after \p maxstrlen bytes, the zero-termination
 * is set and maxstrlen is returned.
 */
int tough_readstr(int fd, char *str, int maxstrlen);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Select one file descriptor \p fd with a timeout of \p to_sec
 * seconds and \p to_usec microseconds.
 */
int select1rw(int fd, bool read_flag, bool write_flag,
	      long to_sec, long to_usec);

/* ---------------------------- Group Separator ---------------------------- */

/*!
 * Ignore the \c SIGPIPE signal.
 *
 * This is usually a good idea, since the default behaviour is to
 * terminate the application. \c SIGPIPE is sent when you try to write
 * to an unconnected socket.  You should check your return codes to
 * make sure you catch this error!
 */
void ignore_pipe(void);

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
