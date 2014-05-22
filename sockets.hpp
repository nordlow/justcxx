/*! \file sockets.hpp
 * \brief Some useful socket handling classes.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cmath>
#include <string>

#include "encode.hpp"
#include "decode.hpp"

/*!
 * Disconnection.
 */
class DisConn
{
public:
protected:
private:
};

/*!
 * Connection Error.
 */
class ConnError
{
public:
protected:
private:
};

/*!
 * Read-Write Selector.
 */
class RWSel
{
public:
    /// Create a selector with a timeout of sec seconds and us microseconds.
    RWSel(uint sec, uint usec = 0)
    {
        reset();
        tot.tv_sec = sec;
        tot.tv_usec = usec;
    }

    void reset()
    {
        FD_ZERO(&rds);
        FD_ZERO(&wrs);
        fdmax = 0;
    }

    void mark_read(int fd)
    {
        FD_SET(fd, &rds);
        fdmax = std::max(fdmax, fd);
    }

    void mark_write(int fd)
    {
        FD_SET(fd, &wrs);
        fdmax = std::max(fdmax, fd);
    }

    int enter()
    {
        struct timeval tmp = tot;
        return ::select(fdmax + 1, &rds, &wrs, 0, &tmp);
    }

    bool awaits_read(int fd) const { return FD_ISSET(fd, &rds); }
    bool awaits_write(int fd) const { return FD_ISSET(fd, &wrs); }

private:
    fd_set rds;			///< Read Set.
    fd_set wrs;			///< Write Set.
    int fdmax;
    struct timeval tot;		///< Timeout.
};

// ============================================================================

/*!
 * Enable / disable blocking of the file descriptor fd.
 *
 * IMPORTANT: It is simplest to use blocking mode TCP-streams. Otherwise the
 * reads and writes can return without having completed the operation.
 */
void set_blocking(int fd, bool do_block);

/// Return true if fd is blocking.
inline
bool is_blocking(int fd)
{
    int x;

    if ((x = fcntl(fd, F_GETFL, 0)) < 0)
    {
        perror("fcntl F_GETFL");
//          return -1;
    }

    return ((x bitand O_NONBLOCK) == 0);
}

class ConnSock;

/*!
 * Socket that listens for TCP-connenctions.
 */
class ListenSock
{
public:
    /// Create a listener on port a.
    ListenSock(int a);

    /// Return the value of the internal file descriptor.
    int get_fd() const { return fd; }

    /// Return true if listener is online, otherwise return false.
    bool is_online() const { return get_fd() > 0; }

    /// Accept a new connection and assign it to cs.
    int accept(ConnSock & cs) throw(int);

    void close()
    {
#ifdef WIN32
        ::closesocket(fd);
#else
        ::close(fd);
#endif
    }

private:
    int bind(int port) throw(int);

    /***
     * Set queue limit qlen for incoming connections.
     * \param[in] ql is the queue length.
     */
    void listen(int ql) { ::listen(fd, ql); }

private:
    int fd;			///< The file descriptor.
};

/*!
 * TCP Stream Connection Socket.
 */
class ConnSock
{
    friend class ListenSock;
public:
    ConnSock();
    ~ConnSock();

    /// Return the value of the internal file descriptor.
    int get_fd() const { return fd; }

    int connect(const std::string & host, int port);
    void disconnect();

    void set_timeout(double t);   // in seconds
    void blockSelect();
    void pollSelect();

    bool is_connected() const { return connflag; }

    ConnSock & operator << (char a);
    ConnSock & operator << (short a);
    ConnSock & operator << (int a);
    ConnSock & operator << (long a);
    ConnSock & operator << (float a);
    ConnSock & operator << (double a);
    ConnSock & operator << (const char * a);
    ConnSock & operator << (const std::string & a);

    template<typename T>
    ConnSock & operator << (const T & a) { ::encode(*this, a); return a; }

    ConnSock & operator >> (char & a);
    ConnSock & operator >> (short & a);
    ConnSock & operator >> (int & a);
    ConnSock & operator >> (long & a);
    ConnSock & operator >> (float & a);
    ConnSock & operator >> (double & a);
    ConnSock & operator >> (char * & a);
    ConnSock & operator >> (std::string & a);

    template<typename T>
    ConnSock & operator >> (T & a) { ::decode(*this, a); return a; }

protected:
private:

    // Default to TCP socket.
    void initialize(int domain = AF_INET,
                    int type = SOCK_STREAM,
                    int protocol = 0);

    // Initialize also specifying the remote IP address (rip).
    void initialize(int sockfd, const std::string & rip);

    /***
     * Read a block data of length size.
     * Assumes blocking mode.
     */
    int blocked_read(char * data, int size) throw(DisConn);

    /***
     * Write a block data of length size.
     * Assumes blocking mode.
     */
    int blocked_write(const char * data, int size) throw(DisConn);

    /// Copy constructor doesn't make sense => Disabled.
    ConnSock(const ConnSock & s);   // \todo OR does it?

    ConnSock(int sockfd, const std::string & rip);
    ConnSock(int domain, int type, int protocol);

    // Specifies the communication semantics. See man socket(2).
    // typedef enum {Stream, DGram, SeqPacket, Raw, Rdm} Type;

    bool initflag:1;
    bool connflag:1;
    bool block_select:1;

    int domain;

    int fd;

    std::string remip;		///< Remote IP-address.
};
