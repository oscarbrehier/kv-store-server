#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <signal.h>
#include <errno.h>

#include "commands.h"
#include "kv_table.h"

#define PORT 8080
#define DEFAULT_BUFFER_SIZE 1024

volatile sig_atomic_t should_exit = 0;
pthread_t *thread_ids = NULL;
int max_threads = 0;
int num_threads = 0;
pthread_mutex_t thread_mutex = PTHREAD_MUTEX_INITIALIZER;

void handle_sigint(int sig)
{
	(void)sig;
    should_exit = 1;
	printf("\nShutdown signal received. Exiting...\n");
}

void cleanup_ressources()
{
	pthread_mutex_lock(&thread_mutex);
	free(thread_ids);
	pthread_mutex_unlock(&thread_mutex);
	pthread_mutex_destroy(&thread_mutex);
}

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
			return (NULL);
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
	int			i;
    int			client_socket;
	char		*buffer;
	ssize_t		valread;
	pthread_t 	tid;

	t_kv_table  *t_table;

	t_table = NULL;

    kv_init_table(&t_table, 8);
	kv_load_file(t_table, "./data/bonjour.kvdb");

	tid = pthread_self();
	client_socket = *(int *)arg;
	free(arg);
	printf("Client connected (threaded).\n");
	while (!should_exit)
	{
		buffer = read_input(&client_socket, &valread);
		if (!buffer)
			break;
        buffer[strcspn(buffer, "\r\n")] = '\0';
		printf("Received %s\n", buffer);
        if (strcmp(buffer, "quit") == 0)
        {
            send(client_socket, "Goodbye!\n", strlen("Goodbye!\n"), 0);
			free(buffer);
            break;
        }
        handle_client_input(client_socket, t_table, buffer);
        // else
        // {
        //     send(client_socket, "Unknown command\n", strlen("Unknown command\n"), 0);
        // }
		free(buffer);
	}
	close(client_socket);
	pthread_mutex_lock(&thread_mutex);
	i = 0;
	while (i < num_threads)
	{
		if (pthread_equal(thread_ids[i], tid))
		{
			if (i < num_threads - 1)
			{
				thread_ids[i] = thread_ids[num_threads - 1];
			}
			num_threads--;
			break;
		}
		i++;
	}
	pthread_mutex_unlock(&thread_mutex);
	pthread_exit(NULL);
}

int main(void)
{
    int 				server_fd;
    int 				*new_socket;
    struct sockaddr_in	address;
    int 				opt;
    socklen_t			addrlen;
	struct sigaction		sa;

	max_threads = 10;
	thread_ids = malloc(sizeof(pthread_t) * max_threads);
	if (!thread_ids)
	{
		perror("Failed to allocate thread tracking");
		return (1);
	}
	sa.sa_handler = handle_sigint;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}
	opt = 1;
	addrlen = sizeof(address);
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket failed");
		free(thread_ids);
        return (1);
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt SO_REUSEADDR");
		free(thread_ids);
        return (1);
    }
    #ifdef SO_REUSEPORT
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt SO_REUSEPORT");
		free(thread_ids);
        return (1);
    }
    #endif
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
		free(thread_ids);
        return (1);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
		free(thread_ids);
        return (1);
    }
    printf("Server listening on port %d\n", PORT);
	if (command_sys_init() != 0)
	{
		close(server_fd);
		cleanup_ressources();
		return 0;
	}
    while (!should_exit)
    {
        new_socket = malloc(sizeof(int));
		if (!new_socket)
		{
			perror("malloc for new_socket");
			continue ;
		}
        *new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
        if (*new_socket < 0)
        {
			if (errno == EINTR)
			{
				free(new_socket);
				break ;
			}
            perror("accept");
            close(*new_socket);
            continue ;
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, new_socket) != 0)
        {
            perror("pthread_create");
            close(*new_socket);
            free(new_socket);
			continue ;
        }
		pthread_mutex_lock(&thread_mutex);
		if (num_threads >= max_threads)
		{
			pthread_t *temp = realloc(thread_ids, sizeof(pthread_t) * max_threads * 2);
			if (temp)
			{
				thread_ids = temp;
				max_threads *= 2;
			}
		}
		if (num_threads < max_threads) {
			thread_ids[num_threads++] = tid;
		}
		pthread_mutex_unlock(&thread_mutex);	
        pthread_detach(tid);
    }
    close(server_fd);
	cleanup_ressources();
	command_sys_cleanup();
    return 0;
}