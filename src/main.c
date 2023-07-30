#include <stdio.h>
#include <pthread.h>

#include "client.h"
#include "layout.h"
#include "config.h"
#include "monitor.h"

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xinerama.h>
#include "keys.h"

Display *display;
Window root;
int running = 1;

Monitor *currentMonitor;

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define LENGTH(x) (sizeof(x) / sizeof(x[0]))

Layout *getCurrentLayout() {
    Layout *current = &currentMonitor->layouts[currentMonitor->currentLayout];
    return current;
}

struct {
    int dragging;
    int diffX;
    int diffY;
    Window window;
} dragWindow;

struct {
    Window window;
    int windowX;
    int windowY;
    int resizing;
} resizeWindow;

void shutdown() {
    running = 0;
}

void changeCursor(Window window, unsigned int cursorId) {
    Cursor cursor;
    cursor = XCreateFontCursor(display, cursorId);
    XDefineCursor(display, window, cursor);
}

void onNewWindow(XMapRequestEvent event) {
    printf("create window\n");
    int width = 300;
    int height = 300;

    Layout *layout = getCurrentLayout();

    createClient(display, layout, event.window, width, height);
    updateLayout(display, layout, 1920, 1080);
}

void *newThread(void *data) {
    char *name = (char *) data;
    printf("new thread: %s\n", name);
    system("DISPLAY=:1 xterm");
}

void handleFirstClickDrag(XEvent *event) {
    // window hovered by mouse cursor
    Window window = event->xbutton.subwindow;

    if (window) {
        XWindowAttributes windowAttributes;
        XGetWindowAttributes(display, window, &windowAttributes);
        XRaiseWindow(display, window);

        dragWindow.diffX = event->xbutton.x_root - windowAttributes.x;
        dragWindow.diffY = event->xbutton.y_root - windowAttributes.y;
        dragWindow.dragging = 1;

        dragWindow.window = window;

        changeCursor(window, XC_fleur);
    }
}

void handleFirstClickResize(XEvent *event) {
    Window window = event->xbutton.subwindow;

    if (window) {
        resizeWindow.resizing = 1;
        resizeWindow.window = window;

        XWindowAttributes windowAttributes;
        XGetWindowAttributes(display, window, &windowAttributes);

        resizeWindow.windowX = windowAttributes.x;
        resizeWindow.windowY = windowAttributes.y;

        changeCursor(window, XC_bottom_right_corner);
    }
}

void commandFunction(void *data) {
    char *command = (char *) data;
    printf("command: %s\n", command);
    system(command);
}

void handleEvents() {
    XEvent event;
    XNextEvent(display, &event);
    static Time lastTime = 0;

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
        if (dragWindow.dragging == 0 && resizeWindow.resizing == 0) {
            Window window = event.xcrossing.window;
            printf("mouse enter\n");
            // print title of the window
            char *title;
            XFetchName(display, window, &title);
            printf("title: %s\n", title);

            XSetInputFocus(display, window, RevertToParent, CurrentTime);

            Client *client = getClientFromWindow(getCurrentLayout(), &window);
            if (client) {
                client->isFocus = 1;
            }
        }
    }

    // if mouse cursor is moved outside a window
    if (event.type == LeaveNotify) {
        if (dragWindow.dragging == 0) {

            Window window = event.xcrossing.window;
            Client *client = getClientFromWindow(getCurrentLayout(), &window);
            if (client) {
                client->isFocus = 0;
            }
        }
    }

    // alt + left click to select which window to drag
    if (event.type == ButtonPress) {
        // check if modifier is super key and left click
        if (event.xbutton.state == SUPER_KEY && event.xbutton.button == 1) {
            handleFirstClickDrag(&event);
        }

        if (event.xbutton.state == SUPER_KEY && event.xbutton.button == 3) {
            handleFirstClickResize(&event);
        }
    }

    // hold alt + left click to drag window
    if (event.type == MotionNotify) {
        if (dragWindow.dragging) {
            XMoveWindow(display, dragWindow.window,
                        event.xmotion.x_root - dragWindow.diffX,
                        event.xmotion.y_root - dragWindow.diffY);


        } else if (resizeWindow.resizing) {
            int width = MAX(40, event.xmotion.x_root - resizeWindow.windowX);
            int height = MAX(40, event.xmotion.y_root - resizeWindow.windowY);

            XResizeWindow(display, resizeWindow.window,
                          width,
                          height);
        } else {
            if (event.xmotion.time - lastTime > 1000 / 30) {
                lastTime = event.xmotion.time;
                int mouseX = event.xmotion.x_root;
                int mouseY = event.xmotion.y_root;

                // for all monitors
                for (int i = 0; i < getMonitorSize(); i++) {
                    Monitor *monitor = &getMonitors()[i];
                    // if mouse cursor is inside monitor
                    if (mouseX >= monitor->x && mouseX <= monitor->x + monitor->width) {
                        currentMonitor = monitor;
                    }
                }
            }
        }
    }

    if (event.type == ButtonRelease) {
        if (resizeWindow.resizing == 1) {
            resizeWindow.resizing = 0;
            XUndefineCursor(display, resizeWindow.window);
        }

        if (dragWindow.dragging == 1) {
            dragWindow.dragging = 0;
            XUndefineCursor(display, dragWindow.window);
        }
    }

    if (event.type == KeyPress) {
        // get key by keycode
        for (int i = 0; i < LENGTH(keys); i++) {
            Key key = keys[i];
            if (event.xkey.keycode == XKeysymToKeycode(display, key.keycode) && event.xkey.state == key.modifier) {
                if (key.function != NULL) {
                    key.function(&event, display);
                } else if (key.command != NULL) {
                    pthread_t thread;
                    pthread_create(&thread, NULL, (void *(*)(void *)) commandFunction, (void *) key.command);
                }
            }
        }

    }

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
    XGrabButton(display, AnyButton, SUPER_KEY, root, True,
                ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                GrabModeAsync,
                GrabModeAsync, None, None);

    for (int i = 0; i < LENGTH(keys); i++) {
        Key key = keys[i];
        if (key.keycode != 0) {
            printf("grab key: %d\n", key.modifier);
            XGrabKey(display, XKeysymToKeycode(display, key.keycode), key.modifier, root, True,
                     GrabModeAsync, GrabModeAsync);
        }
    }

    XSelectInput(display, root,
                 PropertyChangeMask | SubstructureNotifyMask | SubstructureRedirectMask | PointerMotionMask);
}

void initMonitors() {
    if (XineramaIsActive(display)) {
        printf("Xinerama is active\n");
    }

    // get and map all virtual screens
    int screenCount;
    XineramaScreenInfo *screenInfo = XineramaQueryScreens(display, &screenCount);

    printf("screen count: %d\n", screenCount);
    // create array of monitors
    mallocMonitors(screenCount);

    for (int i = 0; i < screenCount; i++) {
        XineramaScreenInfo info = screenInfo[i];
        createMonitor(display, i, info.x_org, info.y_org, info.width, info.height);
    }

    currentMonitor = &monitors[0];
}

int main(void) {
    printf("Shift-WM 1.0 starting..\n");

    if (!(display = XOpenDisplay(0x0))) {
        printf("Cannot open display\n");
        return 1;
    }

    root = DefaultRootWindow(display);
    registerEvents();
    changeCursor(root, XC_left_ptr);

    initMonitors();

    printf("Done!\n");

    while (running) {
        handleEvents();

    }
}