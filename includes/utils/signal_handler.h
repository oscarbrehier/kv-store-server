#ifndef SIGNAL_HANDLER
# define SIGNAL_HANDLER

#include <signal.h>


void    handle_sigint(int sig);
int     setup_signal_handlers(void);

#endif