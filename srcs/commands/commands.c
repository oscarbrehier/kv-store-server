#include "commands.h"
#include "common.h"
#include "kv_table.h"
#include "client.h"
#include "g_table.h"
#include "utils/dynamic_buffer.h"
#include "globals.h"

static t_kv_table	*command_table = NULL;
static t_command	*pending_commands[MAX_COMMANDS];
static int			pending_count = 0;

int command_queue(t_command *cmd) {
    if (pending_count >= MAX_COMMANDS) 
		return (1);
    pending_commands[pending_count++] = cmd;
    return (0);
}

int	command_sys_init(void)
{
	int	i;

	kv_init_table(&command_table, 32);
	if (!command_table)
		return (1);
	i = 0;
	while (i < pending_count)
	{
		command_register(pending_commands[i]);
		i++;
	}
	return (0);
}

void	command_sys_cleanup(void)
{
	if (command_table)
	{
		kv_free_table(command_table);
		command_table = NULL;
	};
}

int	command_register(t_command *cmd)
{
	if (!command_table || !cmd || !cmd->name)
		return (1);
	if (kv_set(command_table, cmd->name, cmd, sizeof(t_command), STRING) != SUCCESS_CODE)
		return (1);
	return (0);
}

t_command	*command_find(const char *name)
{
	void	*command;

	if (!command_table || !name)
		return (NULL);
	if (kv_get(command_table, name, &command, STRING) != SUCCESS_CODE)
		return (NULL);
	return ((t_command *)command);
}

void    command_exec(t_dynamic_buffer **buffer, int argc, char **argv)
{ 
	t_command	*command;
	int			status;

	command = NULL;
	if (argc < 1 || !argv || !argv[0])
		return ;
	command = command_find(argv[0]);
	if (!running)
		return;
	if (!command)
	{
		dynamic_buffer_append(*buffer, "(error) unknown command\n", strlen("(error) unknown command\n"));
		return ;
	}
	if ((argc - 1) != command->arg_count)
	{
		dynamic_buffer_appendf(buffer, "(usage): %s\n", command->usage);
		return ;
	}
	status = command->handler(buffer, argc, argv);
	if (status == 0 && command->type == T_WRITE)
		is_dirty = 1;
}