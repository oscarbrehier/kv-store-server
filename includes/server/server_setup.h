#ifndef SERVER_SETUP_H
# define SERVER_SETUP_H

int	setup_server_socket(int *server_fd, struct sockaddr_in *address);
int	initialize_subsystems(void);
int	handle_new_connection(int server_fd, struct sockaddr_in *address);

#endif