/*!
 * \file client.cpp
 * \brief Client part of test program.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#include <iostream>

#include "sockets.hpp"

int main(int argc, char * argv[])
{

    using namespace std;

    if (argc != 1 + 2)
    {
        cout << "usage: " << argv[0] << " host port " << endl;
        return 0;
    }

    ConnSock conn;

    if (conn.connect(argv[1], atoi(argv[2])) < 0)
    {
	return -1;
    }

    while (true)
    {
	try
	{
	    RWSel sel(1);

	    sel.mark_read(conn.get_fd());

	    sel.enter();

	    if (sel.awaits_read(conn.get_fd()))
	    {
		char ch;
		conn >> ch;
		cout << "got: <" << ch << '>' << endl;
	    }
        }
	catch (DisConn o)
	{
	    conn.disconnect();
	    cout << "disconnected" << endl;
	    break;
	}
	catch (ConnError e)
	{
	    conn.disconnect();
	    cout << "connection error" << endl;
	}
	catch (...)
	{
	    cout << "Caught something" << endl;
	}
    }

    return 0;
}
