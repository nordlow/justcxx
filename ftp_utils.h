/*!
 * \file ftp_utils.h
 * \brief FTP Utility Functions
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-10-27 16:01
 *
 * See: http://en.wikipedia.org/wiki/List_of_FTP_commands, ftplib,
 * http://www.ftprush.com/ftp-mlsd.html,
 * http://www.networksorcery.com/enp/protocol/ftp.htm
 *
 * List of FTP Commands
 * - The \c MLST and \c MLSD commands are intended to standardize the
 *   file and directory information returned by the server-FTP
 *   process.  These commands differ from the \c LIST command in that
 *   the format of the replies is strictly defined although
 *   extensible. Time is given in UTC.
 * - \c MDTM (with extension to set date and UTC time)
 * - \c SIZE
 * - \c XCRC, \c XMD5, \c XSHA1
 */

#pragma once

#ifdef HAVE_FTPLIB_H
#  include <ftplib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
