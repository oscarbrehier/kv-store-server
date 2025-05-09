#include "globals.h"
#include "common.h"
#include "server.h"

void	handle_signal(int sig)
{
	printf("received signal %d, shutting down...", sig);
	if (config != NULL)
	{
		server_stop(config);
	}
	running = 0;
}