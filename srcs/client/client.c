#include "globals.h"
#include "common.h"
#include "client.h"

t_client    *create_client(int fd)
{
    t_client    *client;

    client = (t_client *)malloc(sizeof(t_client));
    if (!client)
    {
        perror("Failed to allocate client structure");
        return (NULL);
    }
    client->fd = fd;
    client->buffer_size = MAX_BUFFER_SIZE;
    client->data_length = 0;
    client->buffer = (char *)malloc(sizeof(char) * client->buffer_size);
    if (!client->buffer)
    {
        perror("Failed to allocate client buffer");
        free(client);
        return (NULL);
    }
    return (client);
}

void    free_client(t_client *client)
{
    if (client)
    {
        if (client->buffer)
            free(client->buffer);
        close(client->fd);
        free(client);
    }
}