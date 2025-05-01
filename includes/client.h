#ifndef CLIENT_H
# define CLIENT_H

// Client handler
char    *read_input(int *client_socket, ssize_t	*valread);
void    *handle_client(void *arg);

void	client_send(int fd, char *format, ...);

#endif