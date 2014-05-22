/*!
 * \file server.cpp
 * \brief Server part of test program.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include <iostream>
#include <cstdlib>

#include "sockets.hpp"

using std::cout;

int main(int argc, char * argv[])
{
    using namespace std;

    if (argc != 1 + 1)
    {
        cout << "usage: " << argv[0] << " port " << endl;
        return 0;
    }

    ListenSock listener(std::atoi(argv[1]));

    if (not listener.is_online())
    {
	return -1;
    }

    ConnSock conn;

    while (true)
    {
	try
	{
	    RWSel sel(1);

	    sel.mark_read(listener.get_fd());

	    if (conn.is_connected())
	    {
		sel.mark_read(conn.get_fd());
	    }

	    sel.enter();

	    if (sel.awaits_read(listener.get_fd()))
	    {
		if (listener.accept(conn) >= 0)
		{
		    cout << "connected" << endl;
		}
	    }

	    if (sel.awaits_read(conn.get_fd()))
	    {
		std::string str;
		conn >> str;
		cout << "got: <" << str << '>' << endl;
	    }
	}
	catch (DisConn o)
	{
	    cout << "disconnected" << endl;
	    conn.disconnect();
	}
	catch (ConnError e)
	{
	    conn.disconnect();
	    cout << "connection error" << endl;
	}
	catch (...)
	{
	    conn.disconnect();
	    cout << "Caught something" << endl;
	}
    }

    return 0;
}
