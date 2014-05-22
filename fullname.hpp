/*! \file fullname.hpp
 * \brief Read full user name on Linux.
 */

#pragma once

#include <sys/types.h>
#include <pwd.h>
#include <cerrno>
#include <cstring>

/* Read \p full_name of a user, given their \p user_name.
 * \return 0 for not found, -1 for error, or 1 for success.
 * \see http://stackoverflow.com/questions/833227/whats-the-easiest-way-to-get-a-users-full-name-on-a-linux-posix-system
 *
 * Copy name to `full_name`, but only up to max-1 chars (max includes trailing
 * '\0'). Note that if the GECOS field contains commas, only up to to (but not
 * including) the first comma is copied, since the commas are a convention to
 * add more than just the name into the field, e.g., room number, phone number, etc. */
inline int get_full_user_name(const char *user_name, char *full_name, size_t max)
{
    errno = 0;

    struct passwd *p = getpwnam(user_name);
    if (p == nullptr && errno == 0)
        return 0;
    if (p == nullptr)
        return -1;
    if (max == 0)
        return 1;

    auto n = strcspn(p->pw_gecos, ",");
    if (n > max - 1)
        n = max - 1;

    memcpy(full_name, p->pw_gecos, n);
    full_name[n] = '\0';

    return 1;
}
