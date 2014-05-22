#include <iostream>
#include <algorithm>
#include <vector>

#include "emerger.hpp"
#include "algorithm_x.hpp"

extern "C"
{
#include "xwin.h"
#include "timing.h"
#include "xplots.h"
#include <X11/keysym.h>
}

using std::cout;
using std::cin;
using std::endl;

typedef uint Type;
#define DIM 11

typedef std::vector<double> Vec;

template<class T, std::size_t N>
void draw(XDpy * dpy, XAnim * xanim,
	  uint w, uint h,
	  const emerger<T, N> & a)
{
    const uint nd = a.get_nd();

    std::vector<Type> d0(nd);
    std::vector<Type> d1(nd);

    a.read_data_along(d0, 0);
    a.read_data_along(d1, 1);

    Vec xd(nd);
    Vec yd(nd);

    pnw::copy(xd, d0);
    pnw::copy(yd, d1);

    x11_plot_autoaxis(dpy, xanim, 0, 0, w, h,
		    &xd[0], &yd[0], nd,
		    "X", "Y");
}

void t_emerger(int w, int h)
{
    XDpy dpy;
    XAnim xanim;
    XEvent event;

    x11_openDpy(&dpy);
    x11_anim_open(&dpy, &xanim, w, h, GPFUN);

    uint nd = 100;
    uint nm = 100;
    emerger<Type, DIM> a(nd, nm);
    a.rrandom_data(0, nd);

    while (TRUE)
    {
	XNextEvent(dpy.dpy, &event);
/*          x_printevent(&event); */
	switch (event.type)
        {
	case MapNotify:
	    break;
	case Expose:
	    x11_anim_back2front(&dpy, &xanim);
	    break;
	case ConfigureNotify:
  	    w = event.xconfigure.width;
            h = event.xconfigure.height;

	    x11_anim_clearback(&dpy, &xanim);
	    draw(&dpy, &xanim, w, h, a);

	    break;
	case KeyPress:

            switch (XKeycodeToKeysym(dpy.dpy, event.xkey.keycode, 0))
            {
            case XK_q:
                goto done;
                break;
            case XK_space:
            {
		x11_anim_clearback(&dpy, &xanim);
		draw(&dpy, &xanim, w, h, a);
		x11_anim_showback(&dpy, &xanim);
		break;
            }
            default:
                break;
            }

            break;
        default:
            break;
	}
    }
 done:

    x11_anim_close(&dpy, &xanim);
    x11_closeDpy(&dpy);
}

int main(int argc, char *argv[])
{
    t_emerger(640, 480);
}
