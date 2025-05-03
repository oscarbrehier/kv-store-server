#include "common.h"
#include "globals.h"

volatile sig_atomic_t	should_exit = 0;

void handle_sigint(int sig)
{
	(void)sig;
    should_exit = 1;
	printf("\nShutdown signal received. Exiting...\n");
}

int	setup_signal_handlers(void)
{
	struct sigaction	sa;

	sa.sa_handler = handle_sigint;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGINT, &sa, NULL) == -1)
	{
		perror("sigaction");
		return (-1);
	}
	return (0);
}