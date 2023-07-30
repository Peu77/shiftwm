#include <malloc.h>
#include "monitor.h"
#include "stdio.h"
#include "layout.h"

void createMonitor(Display *display, int index, int x, int y, int width, int height) {
    printf("create monitor %d: %d\n", width, height);

    Monitor *monitor = malloc(sizeof(Monitor));

    monitor->x = x;
    monitor->y = y;
    monitor->width = width;
    monitor->height = height;
    monitor->layouts = malloc(sizeof(Layout) * LAYOUT_SIZE);
    monitor->currentLayout = 0;

    for (int i = 0; i < LAYOUT_SIZE; i++) {
        monitor->layouts[i].clients = NULL;
        // i to string
        char *name = malloc(sizeof(char) * 2);
        sprintf(name, "%d", i);
        monitor->layouts[i].name = name;
        monitor->layouts[i].monitor = monitor;
    }

    printf("x: %d y: %d\n", x, y);

    monitors[index] = *monitor;
}

void mallocMonitors(int size) {
    monitors = malloc(sizeof(Monitor) * size);
    monitorSize = size;
}

Monitor *getMonitors() {
    return monitors;
}

int getMonitorSize() {
    return monitorSize;
}