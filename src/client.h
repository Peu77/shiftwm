#pragma once

#include <X11/Xlib.h>

typedef struct Layout Layout;
typedef struct Client Client;
struct Client {
    Window window;
    int isFocus;
    Client *next;
    int x;
    int y;
    int width;
    int height;
};

void createClient(Display *display, Layout *layout, Window window, int width, int height);

void removeClient(Layout *layout, Client *client);

void moveClient(Display *display, Client *client, int x, int y);

void printSize(Layout *layout);

Client *getClientFromWindow(Layout *layout, const Window *window);

