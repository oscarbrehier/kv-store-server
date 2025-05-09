#ifndef CLIENT_H
# define CLIENT_H

typedef struct s_client 
{
    int		fd;
	char	*buffer;
	size_t	buffer_size;
	size_t	data_length;
} t_client;

// t_client	*client_create(int fd);
void		free_client(t_client *client);

// Client handler
void	*client_handler(void *arg);
// char    *read_input(int *client_socket, ssize_t	*valread);
// void    *handle_client(void *arg);

void	client_send(int fd, char *format, ...);

#endif