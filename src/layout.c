#include "layout.h"
#include "stdio.h"
#include "client.h"
#include "monitor.h"

void updateLayout(Display *display, Layout *layout, int width, int height) {
    Client *client = layout->clients;

    int i = 0;
    while (client->window != 0) {
        if (client == NULL) {
            printf("client is null\n");
            return;
        }

        Monitor *monitor = layout->monitor;

        moveClient(display, client, monitor->x + i * client->width, client->y);
        i++;
        client = client->next;
    }
}