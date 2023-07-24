#pragma once

#include <X11/Xlib.h>

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

static Client *head;

void createClient(Display *display, Window window, int width, int height);

void removeClient(Client *client);

void moveClient(Display *display, Client *client, int x, int y);

void printSize();

Client *getHead();

Client *getClientFromWindow(const Window *window);