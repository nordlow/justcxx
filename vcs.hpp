/*! \file vcs.hpp
 * \brief United Version Control System (VCS) Interface.
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2011-04-11 13:56
 */

#pragma once
#include <cstdlib>
#include <vector>

/*! Version Control System (File) \em Status Type Code.
 */
typedef enum {
    VCS_modified,               ///< Modified
    VCS_edited = VCS_modified,  ///< Edited
    VCS_obselete                ///< Dirty
} __attribute__ ((packed)) VCS_STATUS_t;

/*! Version Control System \em Backend Type Code.
 */
typedef enum {
    VCS_any_,                   ///< Any.

    VCS_CVS,                    ///< CVS.
    VCS_SVN,                    ///< Subversion.
    VCS_GIT,                    ///< Git.
    VCS_BZR,                    ///< Bazaar.
    VCS_HG,                     ///< Mercurial.
    VCS_DARCS,                  ///< DARCS.

    VCS_undefined_,
} __attribute__ ((packed)) VCS_t;

/*! Get Version Control (VC) \em Status of \p filename under directory \p dir.
 */
int vcs_status(std::vector<VCS_STATUS_t>& status_list_return,
               const char * dir = NULL,
               VCS_t backend = VCS_undefined_);
