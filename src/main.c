#include <stdio.h>
#include <X11/Xlib.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(void) {
    Display *display;
    XWindowAttributes attr;
    XButtonEvent start;
    XEvent ev;

    if (!(display = XOpenDisplay(0x0))) return 1;

    XGrabKey(display, XKeysymToKeycode(display, XStringToKeysym("F1")), Mod1Mask,
             DefaultRootWindow(display), True, GrabModeAsync, GrabModeAsync);

    XGrabButton(display, 1, Mod1Mask, DefaultRootWindow(display), True,
                ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);

    XGrabButton(display, 3, Mod1Mask, DefaultRootWindow(display), True,
                ButtonPressMask | ButtonReleaseMask | PointerMotionMask, GrabModeAsync, GrabModeAsync, None, None);

    start.subwindow = None;
    for (;;) {
        XNextEvent(display, &ev);

        if (ev.type == ButtonRelease && ev.xbutton.subwindow != None)
            printf("Window %ld released\n", ev.xbutton.subwindow);

        if (ev.type == KeyPress && ev.xkey.subwindow != None)
            XRaiseWindow(display, ev.xkey.subwindow);
        else if (ev.type == ButtonPress && ev.xbutton.subwindow != None) {
            XGetWindowAttributes(display, ev.xbutton.subwindow, &attr);
            printf("Window %ld moved to %d,%d\n", ev.xbutton.subwindow, attr.x, attr.y);
            start = ev.xbutton;
        } else if (ev.type == MotionNotify && start.subwindow != None) {
            int xdiff = ev.xbutton.x_root - start.x_root;
            int ydiff = ev.xbutton.y_root - start.y_root;
            XMoveResizeWindow(display, start.subwindow,
                              attr.x + (start.button == 1 ? xdiff : 0),
                              attr.y + (start.button == 1 ? ydiff : 0),
                              MAX(1, attr.width + (start.button == 3 ? xdiff : 0)),
                              MAX(1, attr.height + (start.button == 3 ? ydiff : 0)));
        } else if (ev.type == ButtonRelease)
            start.subwindow = None;
    }
}