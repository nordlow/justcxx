#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>		/* Must be included before xf86dga */
#include <X11/extensions/xf86dga.h>

Display	*dpy;
int screen;
XDGAMode *dga_mode;
int mode_num;
XDGADevice* dga_device;

int main(void)
{
    int i;

    dpy = XOpenDisplay(getenv("DISPLAY"));
    screen = DefaultScreen(dpy);

    dga_mode = XDGAQueryModes(dpy, screen, &mode_num);
    printf("XDGAQueryModes:\n"
           "num: %d\n\n", mode_num);

    for (i = 0; i < mode_num; i++)
    {
        printf("Mode number: %d\n"
               "Corresponding modeline as in XF86Config: %s\n"
               "Vertical refresh rate for the modeline: %f (Hz)\n",
               dga_mode[i].num, dga_mode[i].name,
               dga_mode[i].verticalRefresh);

        printf("Flags:\n");

        if (dga_mode[i].flags & XDGAConcurrentAccess)
            printf("- Concurrent client/server access to framebuffer "
                   "is possible.\n");
        if (dga_mode[i].flags & XDGASolidFillRect)
            printf("- XDGAFillRectangle() is supported.\n");
        if (dga_mode[i].flags & XDGABlitRect)
            printf("- XDGACopyArea() is supported.\n");
        if (dga_mode[i].flags & XDGABlitTransRect)
            printf("- XDGACopyTransparentArea() is supported.\n");
        if (dga_mode[i].flags & XDGAPixmap) {
            printf("- Pixmap is returned when mode is initialized.\n"
                   "  Rendering with Xlib is possible for this mode.\n"
                   "  Pixmap width: %d\n"
                   "  Pixmap height: %d\n",
                   dga_mode[i].pixmapWidth, dga_mode[i].pixmapWidth);
        }

        if (dga_mode[i].flags & XDGAInterlaced)
            printf("- Mode is an interlaced mode.\n");

        if (dga_mode[i].flags & XDGADoublescan)
            printf("- Mode is a doublescan mode.\n");

        printf("Width and Height of framebuffer area: %d x %d \n"
               "Pitch of the framebuffer in bytes: %d\n",
               dga_mode[i].imageWidth, dga_mode[i].imageHeight,
               dga_mode[i].bytesPerScanline);

        printf("Byte order: ");

        if (dga_mode[i].byteOrder == MSBFirst)
            printf("MSBFirst\n");
        else if (dga_mode[i].byteOrder == LSBFirst)
            printf("LSBFirst\n");

        printf("Number of bits in each pixel which contain usable data: %d\n"
               "The number of bits taken up by each pixel: %d\n"
               "RGB masks (non color-indexed modes): 0x%lx 0x%lx 0x%lx\n",
               dga_mode[i].depth, dga_mode[i].bitsPerPixel,
               dga_mode[i].redMask,
               dga_mode[i].greenMask,
               dga_mode[i].blueMask);
        printf("Visual class: ");

        switch (dga_mode[i].visualClass)
        {
        case StaticGray:  printf("StaticGray\n");  break;
        case GrayScale:   printf("GrayScale\n");   break;
        case StaticColor: printf("StaticColor\n"); break;
        case PseudoColor: printf("PseudoColor\n"); break;
        case TrueColor:   printf("TrueColor\n");   break;
        case DirectColor: printf("DirectColor\n"); break;
        }

        printf("Viewport: %d x %d "
               "and its granularity (XDGASetViewport): %d x %d\n"
               "Maximum viewport %d x %d\n",
               dga_mode[i].viewportWidth, dga_mode[i].viewportHeight,
               dga_mode[i].xViewportStep, dga_mode[i].yViewportStep,
               dga_mode[i].maxViewportX, dga_mode[i].maxViewportY);

        if (dga_mode[i].viewportFlags & XDGAFlipRetrace)
            printf("Hardware can switch viewports during "
                   "the vertical retrace.\n");

        if (dga_mode[i].viewportFlags & XDGAFlipImmediate)
            printf("Hardware can switch viewports immediately "
                   "without waiting for the vertical retrace.\n");

        printf("\n");
    }

    /* The client needs sufficient privledges to be able to do this.
       Must be called prior to XDGASetMode. */
    if (not XDGAOpenFramebuffer(dpy, screen)) {
        fprintf(stderr, "Cannot open framebuffer.\n");
        exit(1);
    }

    /* Initialize the "last" DGA Mode */
    dga_device = XDGASetMode(dpy, screen, mode_num);

    /* DGA provides its own events.  These can be enabled by calling
       XDGASelectInput().  This function is similar to XSelectInput() and all
       Xlib Key, Button and Motion masks are supported. */
    XDGASelectInput(dpy, screen, 0xffffffff);
    i = 10000;

    /* Exit DGA mode */
    XDGASetMode(dpy, screen, 0);

    /* Free XDGADevice */
    XFree(dga_device);

    XCloseDisplay(dpy);

    return 0;
}
