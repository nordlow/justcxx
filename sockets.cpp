/*!
 * \file Socket.cpp
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * PF_INET / PF_INET6 designates IPv4 / IPv6 respectively.
 */

#include "sockets.hpp"

extern "C"
{
#include "utils.h"
}

using std::cout;
using std::cin;
using std::cerr;
using std::endl;

// ============================================================================

void set_blocking(int fd, bool do_block)
{
    int x;

    if (do_block)
    {
#ifdef VXWORKS
	x = ~1;
	if ((x = ioctl(fd, FIONBIO, (int)&x)) < 0)
	{
	    perror("ioctl FIONBIO");
	}
#else
	if ((x = fcntl(fd, F_GETFL, 0)) < 0)
	{
	    perror("fcntl F_GETFL");
	}
	x &= ~O_NONBLOCK;
	if (fcntl(fd, F_SETFL, x) < 0)
	{
	    perror("fcntl F_SETFL, O_NONBLOCK");
	}
#endif
    }
    else
    {
#ifdef VXWORKS
	x = 1;
	if ((x = ioctl(fd, FIONBIO, (int)&x)) < 0)
	{
	    perror("ioctl FIONBIO");
	}
#else
	if ((x = fcntl(fd, F_GETFL, 0)) < 0)
	{
	    perror("fcntl F_GETFL");
	}
	x |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, x) < 0)
	{
	    perror("fcntl F_SETFL, O_NONBLOCK");
	}
#endif
    }
}

// ============================================================================

ListenSock::ListenSock(int a)
{
    fd = ::socket(AF_INET, SOCK_STREAM, 0); // domain, type, protocol
    set_blocking(fd, true);
    bind(a);
    listen(32);
}

int ListenSock::bind(int port) throw(int)
{
    struct sockaddr_in servaddr;

    memset(& servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET; // domain is internet sockets
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    int ret = ::bind(fd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    if (ret == -1)
    {
        perror("bind");
	close();
	fd = 0;
    }

    return ret;
}

int ListenSock::accept(ConnSock & cs) throw(int)
{
    cs.disconnect();

    struct sockaddr_in clientaddr;

    uint clientlen = sizeof(clientaddr);
    int new_fd = ::accept(fd, (struct sockaddr *) &clientaddr, &clientlen);
    if (new_fd < 1)
    {
	perror("accept");
        cs.initflag = false;
    } else {
        // statically allocated C-string. no freeing needed.
        const char * ip =
            inet_ntoa(*(struct in_addr *) & clientaddr.sin_addr.s_addr);

#ifndef NDEBUG
        cout << GPFUN << " "
             << ip << endl;
#endif

        // Copy information to function argument.
        cs.initialize(new_fd, ip);
        cs.connflag = true;
    }

    return new_fd;
}

// ============================================================================

ConnSock::ConnSock()
    : initflag(false)
{
    initialize();
}

ConnSock::ConnSock(int domain, int type, int protocol)
    : initflag(false)
{
    initialize(domain, type, protocol);
}

ConnSock::ConnSock(int sockfd, const std::string & rip)
    : initflag(true), fd(sockfd), remip(rip)
{
}

void ConnSock::initialize(int domain, int type, int protocol)
{
    this->domain = domain;
    fd = ::socket(domain, type, protocol);
    initflag = (fd > 0);

    connflag = false;

    set_blocking(fd, true);
}

void ConnSock::initialize(int sockfd, const std::string & rip)
{
    fd = sockfd;
    remip = rip;

    initflag = (fd > 0);

    connflag = false;

    set_blocking(fd, true);
}

ConnSock::~ConnSock()
{
}

int ConnSock::connect(const std::string & hostname, int port)
{
    if (connflag)
    {
        cerr << GPFUN
             << ": Socket is already connected!";
	return -1;
    }

    struct sockaddr_in servaddr;

    memset(& servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = domain;
    servaddr.sin_port = htons(port);
    if (isdigit(hostname[0]))
    {
        servaddr.sin_addr.s_addr = ::inet_addr(hostname.c_str());
    }
    else
    {
        struct hostent * host_struct;
        struct in_addr * host_node;

        host_struct = gethostbyname(hostname.c_str());

        if (host_struct)
        {
            host_node = (struct in_addr*) host_struct->h_addr;
            servaddr.sin_addr.s_addr = host_node->s_addr;
        }
    }

    // Never blocks.
    int ret = ::connect(fd, (struct sockaddr *) &servaddr,
			sizeof(servaddr));
    connflag = (ret >= 0);

    if (ret < 0)
    {
        perror("connect");
    }

    return ret;
}

void ConnSock::disconnect()
{
    if (connflag)
    {
#ifdef WIN32
        ::closesocket(fd);
#else
        ::close(fd);
#endif
        connflag = false;
    }
}

int ConnSock::blocked_read(char * data, int size)
    throw(DisConn)
{
    int ret = ::read(fd, data, size);

    if (ret == 0)
    {
	throw DisConn();
    }
    else if (ret == -1)
    {
	switch (errno)
	{
	case EAGAIN:
	    perror("stubborn_read");
	    disconnect();
	    break;
	default:
	    disconnect();
	    break;
	}
	throw ConnError();
    }

    return ret;
}

int ConnSock::blocked_write(const char * data, int size)
    throw(DisConn)
{
    int ret = ::write(fd, data, size);

    if (ret == 0)
    {
	throw DisConn();
    }
    else if (ret == -1)
    {
	switch (errno)
	{
	case EAGAIN:
	    perror("stubborn_write");
	    disconnect();
	    break;
	default:
	    disconnect();
	    break;
	}
	throw ConnError();
    }

    return ret;
}

ConnSock & ConnSock::operator << (char a)
{
    blocked_write((char *) & a, sizeof(a));
    return *this;
}

ConnSock & ConnSock::operator << (int a)
{
    int net_a = htonl(a);
    blocked_write((char *) & net_a, sizeof(net_a));
    return *this;
}

ConnSock & ConnSock::operator << (long a)
{
    long net_a = htonl(a);
    blocked_write((char *) & net_a, sizeof(net_a));
    return *this;
}

ConnSock & ConnSock::operator << (float a)
{
    long net_a = htonl(* reinterpret_cast<long*>(& a));
    blocked_write((char *) & net_a, sizeof(net_a));
    return *this;
}

ConnSock & ConnSock::operator << (double a)
{
    // \todo FIX byte conversion for this type.
    blocked_write((char *) & a, sizeof(a));
    return *this;
}

ConnSock & ConnSock::operator << (const char * a)
{
    long size = std::strlen(a);
    long net_size = htonl(* reinterpret_cast<long*>(& size));
    blocked_write((const char *) & net_size, sizeof(net_size));
    blocked_write((const char *) a, size);
    return *this;
}

ConnSock & ConnSock::operator << (const std::string & s)
{
    long size = s.size();
    long net_size = htonl(* reinterpret_cast<long*>(& size));
    blocked_write((const char *) & net_size, sizeof(net_size));
    blocked_write((const char *) s.c_str(), size);
    return *this;
}

ConnSock & ConnSock::operator >> (char & a)
{
    blocked_read((char *) & a, sizeof(a));
    return *this;
}

ConnSock & ConnSock::operator >> (int & a)
{
    int net_a;
    blocked_read((char *) &net_a, sizeof(net_a));
    a = ntohl(net_a);
    return *this;
}

ConnSock & ConnSock::operator >> (long & a)
{
    long net_a;
    blocked_read((char *) & net_a, sizeof(net_a));
    a = ntohl(net_a);
    return *this;
}

ConnSock & ConnSock::operator >> (float & a)
{
    blocked_read((char *) & a, sizeof(a));
    return *this;
}

ConnSock & ConnSock::operator >> (double & a)
{
    blocked_read((char *) & a, sizeof(a));
    return *this;
}

ConnSock & ConnSock::operator >> (std::string & a)
{
    long net_size;
    blocked_read((char *) & net_size, sizeof(net_size));
    long host_size = ntohl(net_size);
    a.resize(host_size);
    blocked_read(const_cast<char *>(a.c_str()), host_size);
    return *this;
}
