#include "commands.h"
#include "common.h"
#include "kv_table.h"
#include "client.h"
#include "g_table.h"
#include "utils/dynamic_buffer.h"
#include "globals.h"
#include "client.h"
#include "status_codes.h"
#include "logs.h"

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
	if (kv_set(command_table, cmd->name, cmd, sizeof(t_command), STRING).code != SUCCESS)
		return (1);
	return (0);
}

t_command	*command_find(const char *name)
{
	void		*command;
	t_status	status;

	if (!command_table || !name)
		return (NULL);
	status = kv_get(command_table, name, (void **)&command, STRING);
	if (status.code != SUCCESS)
		return (NULL);
	return ((t_command *)command);
}

void    command_exec(t_dynamic_buffer **buffer, int argc, char **argv, t_client *client)
{ 
	t_command	*command;
	t_status	status;

	command = NULL;
	if (argc < 1 || !argv || !argv[0])
	{
		dynamic_buffer_append(*buffer, "unknown command\n", strlen("unknown command\n"));
		return ;
	}
	command = command_find(argv[0]);
	if (!running)
	{
		dynamic_buffer_append(*buffer, "server shutting down\n", strlen("server shutting down\n"));
		return;
	}
	if (!command)
	{
		dynamic_buffer_append(*buffer, "unknown command\n", strlen("unknown command\n"));
		return ;
	}
	command_logger(*client, *command, argc, argv);
	if (validate_input(buffer, command, client, argc, argv) == FN_ERROR)
	{
		return ;
	}
	status = command->handler(buffer, argc, argv, client);
	alogf(status.log_level, client->ip, client->port, "-> %s", status_messages[status.code]);
	if (status.code == SUCCESS || status.code == WARNING_KEY_EXISTS)
	{
		if (command->flags & T_WRITE)
			is_dirty++;
	}
}