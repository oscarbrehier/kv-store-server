#ifndef COMMANDS_H
# define COMMNADS_H

typedef struct s_kv_table t_kv_table;

typedef void    (*cmd_handler)(t_kv_store *, int, char **);

typedef struct s_command
{
	const char  *name;
    const char  *usage;
    const char  *description;
    cmd_handler handler;
} t_command;

int	        command_sys_init(void);
void	    command_sys_cleanup(void);
int	        command_register(t_command *cmd);
t_command	*command_find(const char *name);
void        command_exec(int socket, t_kv_table *table, int argc, char **argv);

void    handle_client_input(int socket, t_kv_table *table, char *input);
void    parse_input(char *input, int *argc, char ***argv);

#endif