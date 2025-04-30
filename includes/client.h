#ifndef CLIENT_H
# define CLIENT_H

#include <uuid/uuid.h>

typedef struct s_client
{
    char	uuid[37];
    int		socket;
} t_client;

void	client_send(int fd, char *buffer);

#endif