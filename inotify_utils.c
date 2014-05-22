#include "inotify_utils.h"
#include "stdio_x.h"

#include <unistd.h>

/* ---------------------------- Group Separator ---------------------------- */

#ifdef HAVE_SYS_INOTIFY_H
void inotify_print_event(const struct inotify_event *event)
{
  PNOTE("wd=%d mask=%u cookie=%u len=%u,",
	   event->wd, event->mask,
	   event->cookie, event->len);

  if (!(event->mask & (IN_DELETE_SELF | IN_MOVE_SELF | IN_UNMOUNT))) {
    if (event->mask & IN_ISDIR) { printf(" dir:"); } else { printf(" file:"); }
    printf("\"%s\"", event->name);
  }

  if (event->mask & IN_ACCESS) { printf(" was accessed (read)"); }
  else if (event->mask & IN_MODIFY) { printf(" was modifed."); }
  else if (event->mask & IN_ATTRIB) { printf("'s metadata (inode or xattr) was changed."); }
  else if (event->mask & IN_CLOSE_WRITE) { printf(" was closed (previously opened for writing)."); }
  else if (event->mask & IN_CLOSE_NOWRITE) { printf(" was closed (previously opened for writing)."); }
  else if (event->mask & IN_OPEN) { printf(" was opened."); }
  else if (event->mask & IN_MOVED_FROM) { printf(" was moved away from watch."); }
  else if (event->mask & IN_MOVED_TO) { printf(" was moved to watch."); }
  else if (event->mask & IN_CREATE) { printf(" was created at watch."); }
  else if (event->mask & IN_DELETE) { printf(" was deleted from watch."); }
  else if (event->mask & IN_DELETE_SELF) { printf(" watch itself was deleted."); }
  else if (event->mask & IN_MOVE_SELF) { printf(" watch itself was moved."); }
  else if (event->mask & IN_UNMOUNT) { printf(" watch itself was unmounted."); }
  else if (event->mask & IN_IGNORED) { printf(" was ignored."); }
  else if (event->mask & IN_Q_OVERFLOW) { printf(" event queued overflowed."); }

  endline();
}
#endif

#include <poll.h>
#include <sys/ioctl.h>

#ifdef HAVE_SYS_INOTIFY_H
int inotify_process_events(int fd,
			   void *(*event_handler)(void *arg, const struct inotify_event *),
			   void *arg)
{
  int ret = 0;
  struct pollfd ufds[1];
  const uint nfds = 1;          /* one file descriptor to poll for now */
  const int timeout = 0;	/* timeout in milliseconds */

  ufds[0].fd = fd;		/* one to poll */
  ufds[0].events = POLLIN;	/* we check if we can read from it */
  ufds[0].revents = 0;
  int rval = poll(&ufds[0], nfds, timeout); /* check it */
  if (rval == 1) {
    if (ufds[0].revents & POLLIN) {

      /* determine how man bytes that are pending in queue */
      uint qN;
      int l_ret = ioctl(ufds[0].fd, FIONREAD, &qN);
      if (l_ret < 0) { perror("ioctl"); return -1; }
      else { /* printf("%u bytes pending in queue\n", qN); */ }

      char buf[qN];
      int i = 0;			   /* index for events */
      int len = read(ufds[0].fd, buf, qN); /* read number of events */
      if (len < 0) {
        if (errno == EINTR) { PNOTE("Need to reissue system call\n"); }
	else                { perror ("read"); }
	return -1;
      } else if (len == 0) {
	PWARN("qN=%d to small\n", qN);
      } else {
	/* NOTE: inotify require a special way of looping because events
	 * can be of different sizes... */
	ssize_t event_size = sizeof(struct inotify_event);
	while (i < len) {	/* as long as we have events left */
	  struct inotify_event *event = (struct inotify_event *) &buf[i];
	  /* inotify_print_event(event); */
	  event_handler(arg, event);
	  i += (event_size + event->len); /* move index to next event */
	}
      }
    }
  } else if (rval == 0) {
    /* do nothing */
  } else if (rval < 0) {
    lperror("poll");
  }
  return ret;
}

#endif
