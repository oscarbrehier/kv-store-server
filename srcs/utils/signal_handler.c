#include "globals.h"
#include "common.h"

void	handle_signal(int sig)
{
	printf("received signal %d, shutting down...\n", sig);
	running = 0;
}

void	setup_signal(void)
{
	struct sigaction	sa;
	
	sa.sa_handler = handle_signal;
	sa.sa_flags = 0;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}
