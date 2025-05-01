#include "common.h"
#include "globals.h"
#include "g_table.h"
#include "client.h"
#include "commands.h"
#include "threads/thread_manager.h"

char    *read_input(int *client_socket, ssize_t	*valread)
{
    char	*buffer;
    char	*temp_buffer;
    size_t	total_read;
    size_t	buffer_size;

	buffer_size = DEFAULT_BUFFER_SIZE;
	total_read = 0;
	buffer = malloc(sizeof(char) * buffer_size);
	if (!buffer)
	{
		perror("malloc");
		close(*client_socket);
		pthread_exit(NULL);
	}
    while (1)
    {
        *valread = read(*client_socket, buffer + total_read, buffer_size - total_read - 1);
        if (*valread < 0)
        {
			if (errno == EINTR && should_exit)
			{
				free(buffer);
				close(*client_socket);
				pthread_exit(NULL);
			}
            perror("read");
            free(buffer);
            close(*client_socket);
            pthread_exit(NULL);
        }
        else if (*valread == 0)
        {
            free(buffer);
            close(*client_socket);
            pthread_exit(NULL);
        }
        total_read += *valread;
        if (total_read == buffer_size - 1)
        {
            buffer_size *= 2;
            temp_buffer = realloc(buffer, sizeof(char) * buffer_size);
            if (!temp_buffer)
            {
                perror("realloc");
                free(buffer);
                close(*client_socket);
                pthread_exit(NULL);
            }
            buffer = temp_buffer;
        }
        else
        {
            break;
        }
    }
	buffer[total_read] = '\0';
	return (buffer);
}

void    *handle_client(void *arg)
{
    int			client_socket;
	char		*buffer;
	ssize_t		valread;
	pthread_t 	tid;

	tid = pthread_self();
	client_socket = *(int *)arg;
	free(arg);
	printf("Client connected (threaded).\n");
	while (!should_exit)
	{
		buffer = read_input(&client_socket, &valread);
		if (!buffer)
			break ;
        buffer[strcspn(buffer, "\r\n")] = '\0';
        if (strcmp(buffer, "quit") == 0)
        {
            send(client_socket, "OK\n", strlen("OK\n"), 0);
			free(buffer);
            break ;
        }
        handle_client_input(client_socket, buffer);
		free(buffer);
	}
	close(client_socket);
	remove_thread_from_array(tid);
	pthread_exit(NULL);
}
