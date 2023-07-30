#pragma once

#include "config.h"
#include <X11/Xlib.h>


struct Monitor {
    int x;
    int y;
    int width;
    int height;
    struct Layout *layouts;
    int currentLayout;
    Window bar;
};

typedef struct Monitor Monitor;

static Monitor *monitors;
static int monitorSize;

void createMonitor(Display *display, int index, int x, int y, int width, int height);

void mallocMonitors(int size);

Monitor *getMonitors();

int getMonitorSize();