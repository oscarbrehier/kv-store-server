#ifndef SIGNAL_HANDLER
# define SIGNAL_HANDLER

#include <signal.h>

extern volatile sig_atomic_t	should_exit;

void    handle_sigint(int sig);
int     setup_signal_handlers(void);

#endif