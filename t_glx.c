/*!
 * \file t_glx.c
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 *
 * The simplest possible Linux OpenGL program? Maybe...
 *
 * Download the file here: ftp://ftp.artlum.com/pub/simplegl_linux.c
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <GL/glx.h>

#include <X11/Xatom.h>

/*!
 * Something went horribly wrong.
 */
static void
fatalerr(const char *why)
{
  fprintf(stderr, "%s", why);
  exit(0x666);
}

/*!
 * Global vars.
 */
static int g_xscr;
static Atom del_atom;
static Colormap g_cmap;
static Display * g_xdpy;
static XVisualInfo *g_visualinfo;
static GLXContext g_glxcontext;
static Window g_xroot, g_xwin;
static int g_width, g_height;	/* Size of the window */

static int VisData[] = {
  GLX_RGBA,

  GLX_DOUBLEBUFFER,

  GLX_RED_SIZE, 1,
  GLX_GREEN_SIZE, 1,
  GLX_BLUE_SIZE, 1,
  GLX_ALPHA_SIZE, 1,

  GLX_DEPTH_SIZE, 1,
  None
};

/*!
 * Create a window.
 */
static Bool
wait_mapping(Display * d, XEvent * e, char *arg)
{
  return (e->type == MapNotify) && (e->xmap.window == *(Window *) arg);
}

static void
create_win(void)
{
  XEvent event;
  int x, y, attrmask;
  XSizeHints hints;
  XWMHints *StartupState;
  XTextProperty textprop;
  XSetWindowAttributes attr;

  static const char *title = "FTB's little OpenGL example";

  /* Connect to the X server */
  g_xdpy = XOpenDisplay(NULL);
  if (!g_xdpy) {
    fatalerr("Couldn't connect to X server\n");
  }
  g_xscr = DefaultScreen(g_xdpy);
  g_xroot = RootWindow(g_xdpy, g_xscr);

  /* Choose a suitable RGB, double buffered g_visualinfo */
  g_visualinfo = glXChooseVisual(g_xdpy, g_xscr, VisData);
  if (!g_visualinfo) {
    fatalerr("Couldn't get the g_visualinfo!\n");
  }
  if (g_visualinfo->class != TrueColor) {
    fatalerr("g_visualinfo isn't RGB color!\n");
  }

  /* Create a colormap - only needed on some X clients, eg. IRIX */
  g_cmap = XCreateColormap(g_xdpy, g_xroot, g_visualinfo->visual, AllocNone);

  /* Prepare the attributes for our window */
  attr.colormap = g_cmap;
  attr.border_pixel = 0;
  attr.event_mask =
    StructureNotifyMask | EnterWindowMask | LeaveWindowMask | ExposureMask |
    ButtonPressMask | ButtonReleaseMask | OwnerGrabButtonMask | KeyPressMask |
    KeyReleaseMask;
  attrmask = CWColormap | CWBorderPixel | CWEventMask;

  /* Create the window */
  g_width = DisplayWidth(g_xdpy, DefaultScreen(g_xdpy)) / 2;
  g_height = DisplayHeight(g_xdpy, DefaultScreen(g_xdpy)) / 2;
  x = g_width / 2, y = g_height / 2;

  /* Create the window */
  g_xwin = XCreateWindow(g_xdpy, /* Screen */
			 g_xroot,	/* Parent */
			 x, y, g_width, g_height,	/* Position */
			 0,	/* Border */
			 g_visualinfo->depth,	/* Color depth */
			 InputOutput,	/* Class */
			 g_visualinfo->visual,	/* g_visualinfo */
			 attrmask, &attr);	/* Attributes */

  if (!g_xwin) {
    fatalerr("Couldn't create the window\n");
  }

  /* Configure it...  (ok, ok, this next bit isn't "minimal") */
  textprop.value = (unsigned char *) title;
  textprop.encoding = XA_STRING;
  textprop.format = 8;
  textprop.nitems = strlen(title);

  hints.x = x;
  hints.y = y;
  hints.width = g_width;
  hints.height = g_height;
  hints.flags = USPosition | USSize;

  StartupState = XAllocWMHints();
  StartupState->initial_state = NormalState;
  StartupState->flags = StateHint;

  XSetWMProperties(g_xdpy, g_xwin, &textprop, &textprop,
		   /* Window title/icon title              */
		   NULL, 0, /* Argv[], argc for program */
		   &hints,     /* Start position/size */
		   StartupState, /* Iconised/not flag */
		   NULL);
  XFree(StartupState);

  /* Open it, wait for it to appear */
  XMapWindow(g_xdpy, g_xwin);
  XIfEvent(g_xdpy, &event, wait_mapping, (char *) &g_xwin);

  /* Set the kill atom so we get a message when the user tries to close the
   * window */
  if ((del_atom = XInternAtom(g_xdpy, "WM_DELETE_WINDOW", 0)) != None) {
    XSetWMProtocols(g_xdpy, g_xwin, &del_atom, 1);
  }
}

/*!
 * Create the OpenGL rendering context.
 */
static void
create_rctx(void)
{
  /* See if we can do OpenGL on this g_visualinfo */
  int dummy;
  if (!glXQueryExtension(g_xdpy, &dummy, &dummy)) {
    fatalerr("OpenGL not supported by X server\n");
  }

  /* Create the OpenGL rendering context */
  g_glxcontext = glXCreateContext(g_xdpy, g_visualinfo, 0, 1);
  if (!g_glxcontext) {
    fatalerr("Failed to create a GL context\n");
  }

  /* Make it current */
  if (!glXMakeCurrent(g_xdpy, g_xwin, g_glxcontext)) {
    fatalerr("glXMakeCurrent failed for window\n");
  }
}

static void
resize(int width, int height)
{
  printf("%d %d\n", width, height);
  /* Set the view parameters */
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  glOrtho(0, width, 0, height, -1, 1);
}

/*!
 * Window message queue.
 */
static int
update_mq(void)
{
  XEvent event;
  XConfigureEvent *xc;

  while (XPending(g_xdpy)) {
    XNextEvent(g_xdpy, &event);
    switch (event.type) {
    case ClientMessage:
      if ((int)event.xclient.data.l[0] == (int)del_atom) {
	return 0;
      }
      break;
    case ConfigureNotify:
      xc = &(event.xconfigure);
      g_width = xc->width;
      g_height = xc->height;
      resize(g_width, g_height);
      break;
    }
  }

  return 1;
}

/*!
 * Redraw the window.
 */
static void
redraw_win(void)
{
  int size;
  static float a = 0;

  /* Clear the screen */
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* A rotating square */
  a += 0.1;
  glTranslatef((float) g_width / 2, (float) g_height / 2, 0);
  glRotatef(a, 0, 0, 1);
  glBegin(GL_POLYGON);
  size = g_height / 3;
  glColor3f(1, 0, 0);
  glVertex2f(size, size);
  glColor3f(0, 1, 0);
  glVertex2f(-size, size);
  glColor3f(0, 0, 1);
  glVertex2f(-size, -size);
  glColor3f(1, 1, 1);
  glVertex2f(size, -size);
  glEnd();

  /* Swapbuffers */
  glXSwapBuffers(g_xdpy, g_xwin);
}

int
main(void)
{
  create_win();
  create_rctx();

  while (update_mq()) {
    redraw_win();
  }
  return 0;
}
