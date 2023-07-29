#include <stdio.h>
#include <X11/Xlib.h>
#include <stdlib.h>
#include <pthread.h>
#include "client.h"
#include "layout.h"

Display *display;
Window root;

Layout layout[10];
int currentLayout = 0;


Layout *getCurrentLayout() {
    return &layout[currentLayout];
}

struct {
    XButtonEvent buttonEvent;
    XWindowAttributes windowAttributes;
} dragWindow;

void onNewWindow(XMapRequestEvent event) {
    printf("create window\n");
    int width = 300;
    int height = 300;

    createClient(display, getCurrentLayout(), event.window, width, height);
    updateLayout(display, getCurrentLayout(), 1920, 1080);
}

void handleEvents() {
    XEvent event;
    XNextEvent(display, &event);

    // if window is created
    if (event.type == MapRequest) {
        onNewWindow(event.xmaprequest);
    }

    // if window is destroyed
    if (event.type == UnmapNotify) {
        printf("unmap window\n");
        Window window = event.xunmap.window;
        Client *client = getClientFromWindow(getCurrentLayout(), &window);
        if (client != NULL) {
            removeClient(getCurrentLayout(), client);
            updateLayout(display, getCurrentLayout(), 1920, 1080);
        }
    }

    // if mouse cursor is moved inside a window
    if (event.type == EnterNotify) {
        if (dragWindow.buttonEvent.subwindow == None) {
            printf("mouse enter\n");
            // print title of the window
            char *title;
            XFetchName(display, event.xcrossing.window, &title);
            printf("title: %s\n", title);

            XSetInputFocus(display, event.xcrossing.window, RevertToParent, CurrentTime);
        }
    }

    // if mouse cursor is moved outside a window
    if (event.type == LeaveNotify) {
        if (dragWindow.buttonEvent.subwindow == None) {
            printf("mouse leave\n");
        }
    }

    // alt + left click to select which window to drag
    if (event.type == ButtonPress) {
        // get window hovered by mouse cursor
        Window window = event.xbutton.subwindow;
        if (window) {
            XGetWindowAttributes(display, window, &dragWindow.windowAttributes);
            XRaiseWindow(display, window);

            dragWindow.buttonEvent = event.xbutton;
        }
    }

    // hold alt + left click to drag window
    if (event.type == MotionNotify) {
        XButtonEvent buttonEvent = dragWindow.buttonEvent;
        XWindowAttributes windowAttributes = dragWindow.windowAttributes;

        int diffX = event.xmotion.x_root - buttonEvent.x;
        int diffY = event.xmotion.y_root - buttonEvent.y;

        XMoveWindow(display, buttonEvent.subwindow,
                    windowAttributes.x + diffX,
                    windowAttributes.y + diffY);
    }

    if (event.type == ButtonRelease) {
        dragWindow.buttonEvent.subwindow = None;
    }
}

void *newThread(void *data) {
    char *name = (char *) data;
    printf("new thread: %s\n", name);
    system("DISPLAY=:1 alacritty");
}

void drawableWindow() {
    //                                          parent ,x, y, width, height,border_width, border_color, background_color
    Window window = XCreateSimpleWindow(display, root, 0, 0, 500, 500, 2, 0xe81a0c, 0x0000ff);


    XMapWindow(display, window);
    XRaiseWindow(display, window);
    XSetWindowBackgroundPixmap(display, window, None);

    // get graphics context and draw
    GC gc = XCreateGC(display, window, 0, 0);
// set color to red
    XSetForeground(display, gc, 0xe81a0c);
    // change width of line
    XSetLineAttributes(display, gc, 5, LineSolid, CapRound, JoinRound);
    XDrawLine(display, window, gc, 0, 0, 500, 500);

    // draw text
    XDrawString(display, window, gc, 10, 10, "Hello, World!", 14);

}

void registerEvents() {
    XGrabButton(display, 1, Mod1Mask, root, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                GrabModeAsync,
                GrabModeAsync, None, None);

    XSelectInput(display, root, PropertyChangeMask | SubstructureNotifyMask | SubstructureRedirectMask);
}

int main(void) {
    printf("Shift-WM 1.0 starting..\n");

    if (!(display = XOpenDisplay(0x0))) {
        printf("Cannot open display\n");
        return 1;
    }

    pthread_t thread;
    pthread_create(&thread, NULL, newThread, "cool");

    root = DefaultRootWindow(display);
    registerEvents();

    printf("Done!\n");

    while (1) {
        handleEvents();
    }
}