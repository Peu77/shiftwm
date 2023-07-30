#include <X11/keysym.h>
#include "X11/Xlib.h"
#include "stdio.h"
#include "config.h"

void closeWindow(XEvent *event, Display *display) {
    Window window = event->xkey.subwindow;
    if (window) {
        XDestroyWindow(display, window);
    }
    printf("close window\n");
}


typedef struct {
    unsigned int modifier;
    unsigned long keycode;

    void (*function)(XEvent *event, Display *display);

    char *command;
} Key;

void shutdown();

Key keys[] = {
        {SUPER_KEY|ShiftMask, XK_Return, .command = "rofi -show drun"},
        {SUPER_KEY, XK_Return, .command = "alacritty"},
        {SUPER_KEY, XK_b, .command = "brave"},

        {SUPER_KEY|ShiftMask, XK_c, .function = closeWindow},
        {SUPER_KEY, XK_q, .function = shutdown},

};