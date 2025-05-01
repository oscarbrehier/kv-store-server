#include "common.h"
#include "globals.h"
#include "server/server_setup.h"
#include "utils/cleanup.h"
#include "utils/signal_handler.h"
#include "threads/thread_manager.h"

int main(void)
{
    int					server_fd;
	int					result;
	struct sockaddr_in	address;

	if (initialize_thread_resources() != 0)
		return (1);
	if (setup_signal_handlers() != 0)
	{
		free(thread_ids);
		return (1);
	}
	if (setup_server_socket(&server_fd, &address) != 0)
	{
		free(thread_ids);
		return (1);
	}
	if (initialize_subsystems() != 0)
	{
		close(server_fd);
		free(thread_ids);
		return (1);
	}
	while (!should_exit)
	{
		result = handle_new_connection(server_fd, &address);
		if (result == 1)
			break ;
	}
	wait_for_threads();
	close(server_fd);
	cleanup_ressources();
	return (0);
}