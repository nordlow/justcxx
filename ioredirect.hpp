/*! \file ioredirect.hpp
 * \brief IO Redirection.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-04-11 12:46
 */

#pragma once

/*! Execute/Spawn Process \p filepath.
 *
 * \param[in] working_directory is current working directory for child process.
 * \param[in] argv Arguments to \p filepath.
 * \param[in] envp Environment for \p filepath.
 *
 * \param[out] child_pid Process id.
 * \param[out] in is standard input
 * \param[out] err is standard output
 * \param[out] out is standard error
 *
 * \see http://developer.gnome.org/glib/stable/glib-Spawning-Processes.html
 * \see \c g_spawn_async_with_pipes
 *
 * \example
 * const char * argv[] = { "hg", "status", "-S", nullptr };
 * int ret = spawn("/usr/bin/hg", path, * const_cast<char* const*>(argv), const_cast<char* const*>(envp), * &child, nullptr, &out, &err);
 */
int spawn(const char* filepath,
          const char* working_directory = nullptr,
          char *const argv[] = nullptr,
          char *const envp[] = nullptr,
          pid_t *child_pid = nullptr,
          int *in = nullptr,
          int *out = nullptr,
          int *err = nullptr);
