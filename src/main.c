#include <stdio.h>
#include <X11/Xlib.h>
#include "client.h"

Display *display;
Window root;


void updateWindows() {
    Client *client = getHead();

    int i = 0;
    while (client->window != 0) {
        if (client == NULL) {
            printf("client is null\n");
            return;
        }

        moveClient(display, client, i * client->width, client->y);
        i++;
        client = client->next;
    }
}

void onNewWindow(XMapRequestEvent event) {
    printf("create window\n");
    int width = 300;
    int height = 300;

    createClient(display, event.window, width, height);
    updateWindows();
}

void handleEvents() {
    XEvent event;
    XNextEvent(display, &event);

    if (event.type == MapRequest) {
        onNewWindow(event.xmaprequest);
    }

    if (event.type == UnmapNotify) {
        printf("unmap window\n");
        Window window = event.xunmap.window;
        Client *client = getClientFromWindow(&window);
        if (client != NULL) {
            removeClient(client);
            updateWindows();
        }
    }
}

int main(void) {
    printf("Shift-WM 1.0 starting..\n");

    if (!(display = XOpenDisplay(0x0))) {
        printf("Cannot open display\n");
        return 1;
    }

    root = DefaultRootWindow(display);

    XGrabButton(
            //Display, Button, Modifiers
            display, AnyButton, AnyModifier,
            //Window, OwnerE?, EventMask
            root, True, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | OwnerGrabButtonMask,
            //PointerMode, KBMode, Confine, Cursor
            GrabModeAsync, GrabModeAsync, None, None
    );

    XSelectInput(display, root, FocusChangeMask | PropertyChangeMask |
                                SubstructureNotifyMask | SubstructureRedirectMask |
                                KeyPressMask | ButtonPressMask);

    printf("Done!\n");

    while (1) {
        handleEvents();
    }
}