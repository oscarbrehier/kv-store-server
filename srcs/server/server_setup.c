#include "globals.h"
#include "common.h"
#include "g_table.h"
#include "client.h"
#include "threads/thread_manager.h"
#include "utils/cleanup.h"
#include "commands.h"

int	setup_server_socket(int *server_fd, struct sockaddr_in *address)
{
	int	opt;

	opt = 1;
	*server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (*server_fd < 0)
    {
        perror("socket failed");
        return (-1);
    }
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt SO_REUSEADDR");
		close(*server_fd);
        return (-1);
    }
#ifdef SO_REUSEPORT
    if (setsockopt(*server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0)
    {
        perror("setsockopt SO_REUSEPORT");
		close(*server_fd);
        return (-1);
    }
#endif
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(PORT);
    if (bind(*server_fd, (struct sockaddr*)address, sizeof(*address)) < 0)
    {
        perror("bind failed");
		close(*server_fd);
        return (-1);
    }
    if (listen(*server_fd, 3) < 0) {
        perror("listen");
		close(*server_fd);
        return (-1);
    }
    printf("Server listening on port %d\n", PORT);
	return (0);
}

int	initialize_subsystems(void)
{
	if (command_sys_init() != 0)
	{
		fprintf(stderr, "Failed to initalize command system\n");
		return (-1);
	}
	if (init_global_table() != 0)
	{
		perror("table init");
		command_sys_cleanup();
		return (-1);
	}
	if (pthread_create(&autosave_thread, NULL, g_table_autosave, NULL) != 0)
	{
		perror("autosave thread");
		command_sys_cleanup();
		return (-1);
	}
	return (0);
}

int	handle_new_connection(int server_fd, struct sockaddr_in *address)
{
	int	*new_socket;
	socklen_t	addrlen;
	pthread_t	tid;

	addrlen = sizeof(address);
	new_socket = malloc(sizeof(int));
	if (!new_socket)
	{
		perror("malloc for new_socket");
		return (-1);
	}
	*new_socket = accept(server_fd, (struct sockaddr*)&address, &addrlen);
	if (*new_socket < 0)
	{
		if (errno == EINTR)
		{
			free(new_socket);
			return (1);
		}
		perror("accept");
		close(*new_socket);
		return (-1);
	}
	if (pthread_create(&tid, NULL, handle_client, new_socket) != 0)
	{
		perror("pthread_create");
		close(*new_socket);
		free(new_socket);
		return (-1);
	}
	if (add_thread_to_array(tid) != 0)
	{
		fprintf(stderr, "Warning: Failed to add thread to tracking array\n");
	}
	// pthread_detach(tid);
	return (0);
}