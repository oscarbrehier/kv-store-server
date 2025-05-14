#ifndef CLIENT_H
# define CLIENT_H

#include "common.h"

typedef struct s_client
{
    int         socket;
    char        ip[INET_ADDRSTRLEN];
    uint16_t    port;
} t_client;

void	*client_handler(void *arg);

#endif