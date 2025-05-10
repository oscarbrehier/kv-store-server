#ifndef COMMANDS_H
# define COMMANDS_H

#define MAX_COMMANDS 64

typedef struct s_kv_table t_kv_table;
typedef struct s_dynamic_buffer t_dynamic_buffer;
typedef int		(*cmd_handler)(t_dynamic_buffer **, int, char **);

#define T_READ 1
#define T_WRITE 2

typedef struct s_command
{
	const char  *name;
    const char  *usage;
    const char  *description;
	int			type;
	int			arg_count;
    cmd_handler handler;
} t_command;

int command_queue(t_command *cmd);

#define DEFINE_COMMAND(handler_fn, name_str, usage_str, desc_str, arg_count_int, type_int) \
	static t_command name_str##_cmd = { \
		.name = #name_str, \
		.usage = usage_str, \
		.description = desc_str, \
		.handler = handler_fn, \
		.arg_count = arg_count_int, \
		.type = type_int, \
	}; \
	__attribute__((constructor)) \
	static void register_##name_str##_command(void) { \
		command_queue(&name_str##_cmd); \
	}

int	        command_sys_init(void);
void	    command_sys_cleanup(void);
int	        command_register(t_command *cmd);
t_command	*command_find(const char *name);
void        command_exec(t_dynamic_buffer **buffer, int argc, char **argv);

void    handle_client_input(int socket, char *input);
void    parse_input(char *input, int *argc, char ***argv);
void	free_argv(char **argv);

#endif