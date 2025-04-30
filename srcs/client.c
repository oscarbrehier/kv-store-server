#include "common.h"

void	client_send(int fd, char *buffer)
{
	size_t	len;
	char	*message;

	if (!buffer)
		return;
	len = strlen(buffer);
	message = malloc(sizeof(char) * (len + 2));
	if (!message)
		return ;
	memcpy(message, buffer, len);
	message[len] = '\n';
	message[len + 1] = '\0';
	send(fd, message, strlen(message) + 1, 0);
	free(message);
}