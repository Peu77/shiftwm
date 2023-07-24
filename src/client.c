#include "client.h"
#include "malloc.h"

Client *getClientFromWindow(const Window *window) {
    Client *client = getHead();
    while (client != NULL) {
        if (client->window == *window) {
            return client;
        }
        client = client->next;
    }
    return NULL;
}

void createClient(Display *display, Window window, int width, int height) {
    Client *newClient;
    newClient = malloc(sizeof(Client));
    newClient->window = window;
    newClient->isFocus = 1;

    if (getHead() == NULL) {
        head = malloc(sizeof(Client));
        head->window = 0;
    }

    newClient->next = head;
    head = newClient;

    newClient->width = width;
    newClient->height = height;
    XMapWindow(display, window);
    XRaiseWindow(display, window);

    XMoveResizeWindow(display, window,
                      0, 0, width, height);

    printSize();

}

void removeClient(Client *client) {
    Client *current = getHead();

    while (current->window != 0) {
        if (current == client) {
            head = current->next;
            free(client);
            printSize();
            return;
        }

        if (current->next == client) {
            current->next = current->next->next;
            printf("delete client\n");

            free(client);
            printSize();
            return;
        }
        current = current->next;
    }
}

void printSize() {
    Client *client = getHead();
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

Client *getHead() {
    return head;
}