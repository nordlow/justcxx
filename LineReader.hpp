/*!
 * \file LineReader.hpp
 * \brief Wrapper for GNU readline and history.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 */

#pragma once

#include <iostream>
#include <cstring>
#include <cstdio>

extern "C"
{
#include <readline/readline.h>
#include <readline/history.h>
}

/*!
 * LineReader with built-in history.
 */
class LineReader
{
public:
    /// Construct a LineReader with the prompt p.
    LineReader(const char * p) : prompt(p) { ::using_history(); }

    /***
     * Read a line.
     *
     * Blocks until the user has pressed Return or Ctrl-D.
     * \return true if the user entered something, false otherwise (Ctrl-D).
     */
    bool into(std::string & line) const;

public:
    /// Add the string str as a line in the history.
    void add_history(const char * str) const { ::add_history(str); }

    /// Clear the history.
    void clear_history() const { ::clear_history(); }

    /// Read history from file_name.
    void read_history(const char * file_name) const
    {
	::read_history(file_name);
    }

    /// Write history to file_name.
    void write_history(const char * file_name) const
    {
        ::write_history(file_name);
    }

    /// Print the history.
    void print_history(std::ostream & os) const;

private:
    const char * prompt;	///< The prompting C-string.
};
