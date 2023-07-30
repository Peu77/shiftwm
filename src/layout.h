#pragma once

#include <X11/Xlib.h>

typedef struct Client Client;
typedef struct Monitor Monitor;
struct Layout {
    char *name;
    Client *clients;
    Monitor *monitor;
};

typedef struct Layout Layout;


void updateLayout(Display *display, Layout *layout, int width, int height);

