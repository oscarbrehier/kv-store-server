#ifndef COMMANDS_H
# define COMMANDS_H

#define MAX_COMMANDS 64

typedef struct s_status t_status;
typedef struct s_kv_table t_kv_table;
typedef struct s_client t_client;
typedef struct s_dynamic_buffer t_dynamic_buffer;
typedef t_status (*cmd_handler)(t_dynamic_buffer **, int, char **, ...);

#define NO_FLAG (1 << 0)
#define T_READ (1 << 1)
#define T_WRITE (1 << 2)
#define AUTH (1 << 3)
#define NO_AUTH (1 << 4)

typedef struct s_arg
{
	int	hide;
	int min_len;
	int	max_len;
} t_arg;

typedef struct s_command
{
	const char  *name;
    const char  *usage;
    const char  *description;
	int			flags;
	int			arg_count;
    cmd_handler handler;
	t_arg		*args;
} t_command;

int command_queue(t_command *cmd);

#define DEFINE_COMMAND(handler_fn, name_str, usage_str, desc_str, arg_count_int, flags_int, args_list) \
	static t_command name_str##_cmd = { \
		.name = #name_str, \
		.usage = usage_str, \
		.description = desc_str, \
		.handler = handler_fn, \
		.arg_count = arg_count_int, \
		.flags = flags_int, \
		.args = args_list \
	}; \
	__attribute__((constructor)) \
	static void register_##name_str##_command(void) { \
		command_queue(&name_str##_cmd); \
	}

int	        command_sys_init(void);
void	    command_sys_cleanup(void);
int	        command_register(t_command *cmd);
t_command	*command_find(const char *name);
void        command_exec(t_dynamic_buffer **buffer, int argc, char **argv, t_client *client);
void    	command_logger(t_client client, t_command cmd, int argc, char **argv, t_status status);

int			input_parse(char *input, int *argc, char ***argv, t_dynamic_buffer **buffer);
int			input_is_valid_char(char c);
int			validate_input(t_dynamic_buffer **buffer, t_command *command, t_client *client, int argc, char **argv);

void		free_argv(char **argv);

#endif