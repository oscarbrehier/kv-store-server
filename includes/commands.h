#ifndef COMMANDS_H
# define COMMANDS_H

#define MAX_COMMANDS 64

typedef struct  s_kv_table t_kv_table;
typedef void	(*cmd_handler)(int, int, char **);

typedef struct s_command
{
	const char  *name;
    const char  *usage;
    const char  *description;
	int			arg_count;
    cmd_handler handler;
} t_command;

int queue_command(t_command *cmd);

#define DEFINE_COMMAND(handler_fn, name_str, usage_str, desc_str, arg_count_int) \
	static t_command name_str##_cmd = { \
		.name = #name_str, \
		.usage = usage_str, \
		.description = desc_str, \
		.handler = handler_fn, \
		.arg_count = arg_count_int \
	}; \
	__attribute__((constructor)) \
	static void register_##name_str##_command(void) { \
		queue_command(&name_str##_cmd); \
	}

int	        command_sys_init(void);
void	    command_sys_cleanup(void);
int	        command_register(t_command *cmd);
t_command	*command_find(const char *name);
void        command_exec(int socket, int argc, char **argv);

void    handle_client_input(int socket, char *input);
void    parse_input(char *input, int *argc, char ***argv);
void	free_argv(char **argv);

#endif