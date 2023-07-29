#include "client.h"
#include "malloc.h"

#include "layout.h"

Client *getClientFromWindow(Layout *layout, const Window *window) {
    Client *client = layout->clients;
    while (client != NULL && client->window) {
        if (client->window == *window) {
            return client;
        }
        client = client->next;
    }

    return NULL;
}

void createClient(Display *display, Layout *layout, Window window, int width, int height) {
    Client *newClient;
    newClient = malloc(sizeof(Client));

    newClient->window = window;
    newClient->isFocus = 1;

    // set border width and color
    XSetWindowBorder(display, window, 0xe81a0c);
    XSetWindowBorderWidth(display, window, 2);

    // set focus and unfocus event
    XSelectInput(display, window, FocusChangeMask | EnterWindowMask | LeaveWindowMask);

    // create linked-list head if it is the first window
    Client *head = layout->clients;
    if (head == NULL) {
        head = malloc(sizeof(Client));
        head->window = 0;
    }

    // add client to linked-list
    newClient->next = head;
    layout->clients = newClient;

    // set client position, size and show it on the top
    newClient->width = width;
    newClient->height = height;
    XMapWindow(display, window);
    XRaiseWindow(display, window);
    XMoveResizeWindow(display, window,
                      0, 0, width, height);

    printSize(layout);

}

void removeClient(Layout *layout, Client *client) {
    Client *current = layout->clients;

    while (current->window != 0) {
        if (current == client) {
            layout->clients = current->next;
            free(client);
            printSize(layout);
            return;
        }

        if (current->next == client) {
            current->next = current->next->next;
            printf("delete client\n");

            free(client);
            printSize(layout);
            return;
        }
        current = current->next;
    }
}

void printSize(Layout *layout) {
    Client *client = layout->clients;
    int size = 0;
    while (client->window != 0) {
        size++;
        printf("client: %lu\n", client->window);
        client = client->next;
    }

    printf("size: %d\n", size);
}

void moveClient(Display *display, Client *client, int x, int y) {
    client->x = x;
    client->y = y;
    XMoveWindow(display, client->window, x, y);
}