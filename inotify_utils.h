/*!
 * \file inotify_utils.h
 * \brief Inotify Utilities
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2008-02-05 13:33
 *
 * Event Name		Is an Event	Description
 * \c IN_ACCESS		Yes	file was \em accessed.
 * \c IN_ATTRIB		Yes	\em metadata \em changed.
 * \c IN_CLOSE_NOWRITE	Yes	unwrittable file was \em closed.
 * \c IN_CLOSE_WRITE	Yes	writtable file was \em closed.
 * \c IN_CREATE		Yes	file/dir was \em created in watched directory.
 * \c IN_DELETE		Yes	file/dir was \em deleted in watched directory.
 * \c IN_DELETE_SELF	Yes	watched item itself was \em deleted.
 * \c IN_DONT_FOLLOW	No	don't follow a \em symlink (lk 2.6.15).
 * \c IN_IGNORED	Yes	raised on watched item removing. Probably useless for you, prefer instead \c IN_DELETE*.
 * \c IN_ISDIR		No	event occurred against directory. It is always piggybacked to an event. The Event structure automatically provide this information (via .is_dir)
 * \c IN_MASK_ADD	No	to update a mask without overwriting the previous value (lk 2.6.14). Useful when updating a watch.
 * \c IN_MODIFY		Yes	file was modified.
 * \c IN_MOVE_SELF	Yes	watched item itself was moved, currently its full pathname destination can only be traced if its source directory and destination directory are both watched. Otherwise, the file is still being watched but you cannot rely anymore on the given path (.path)
 * \c IN_MOVED_FROM	Yes	file/dir in a watched dir was moved from X. Can trace the full move of an item when IN_MOVED_TO is available too, in this case if the moved item is itself watched, its path will be updated (see IN_MOVE_SELF).
 * \c IN_MOVED_TO	Yes	file/dir was moved to Y in a watched dir (see IN_MOVE_FROM).
 * \c IN_ONLYDIR	No	only watch the path if it is a directory (lk 2.6.15). Usable when calling .add_watch.
 * \c IN_OPEN		Yes	file was opened.
 * \c IN_Q_OVERFLOW	Yes	event queued overflowed. This event doesn't belongs to any particular watch.
 * \c IN_UNMOUNT	Yes	backing fs was unmounted. Notified to all watches located on this fs.
 *
 * \see http://jdennis.fedorapeople.org/lwatch/html/InotifyOverview.html
 * \see http://www.kernel.org/doc/man-pages/online/pages/man7/inotify.7.html
 *
 * The following interfaces can be used to limit the amount of kernel memory
 * consumed by inotify:
 *
 * /proc/sys/fs/inotify/max_queued_events
 * The value in this file is used when an application calls
 * inotify_init(2) to set an upper limit on the number of events that can
 * be queued to the corresponding inotify instance.  Events in excess of
 * this limit are dropped, but an IN_Q_OVERFLOW event is always generated.
 *
 * /proc/sys/fs/inotify/max_user_instances
 * This specifies an upper limit on the number of inotify instances that
 * can be created per real user ID.
 *
 * /proc/sys/fs/inotify/max_user_watches
 * This specifies an upper limit on the number of watches that can be
 * created per real user ID.
 */

/* ========================================================================= */

#pragma once

#if defined(HAVE_CONFIG_H)
#  include "config.h"
#endif

#ifdef HAVE_SYS_INOTIFY_H
#  include <sys/inotify.h>

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * Process inotify events on \p fd calling \p event_handler with
 * argument \p arg for every event.
 */
int inotify_process_events(int fd,
			   void *(*event_handler)(void *arg, const struct inotify_event *),
			   void *arg);

/*!
 * Print inotify event \p event.
 */
void inotify_print_event(const struct inotify_event *event);

#endif

/* ========================================================================= */

#ifdef __cplusplus
}
#endif
