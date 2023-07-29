#pragma once

#include <X11/Xlib.h>

typedef struct Client Client;
struct Layout {
    char *name;
    Client *clients;
};

typedef struct Layout Layout;


void updateLayout(Display *display, Layout *layout, int width, int height);

