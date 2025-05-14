#include "status_codes.h"
#include "logs.h"
#include "common.h"
#include "client.h"
#include "utils/utils.h"

char	*get_log_level(int level)
{
	switch (level)
    {
        case LOG_DEBUG:   return "debug";
        case LOG_INFO:    return "info";
        case LOG_WARNING: return "warning";
        case LOG_ERROR:   return "error";
        case LOG_FATAL:   return "fatal";
        default:          return "unknown";
    }
}

void    alog(int log_level, const char *address, uint16_t port, const char *message)
{
    char	*timestamp;
    char	format[128];
	char	*buffer;
	int		buffer_len;
	int		written;

	if (address != NULL)
	{
		snprintf(format, sizeof(format), "[%%s] [%%s] [%%s:%%d] %%s\n");
	}
	else
	{
		snprintf(format, sizeof(format), "[%%s] [%%s] %%s\n");
	}
    timestamp = get_current_timestamp(time(NULL));
    if (!timestamp)
	{
		fprintf(stderr, "Failed to generate log timestamp\n");
		return ;
	}
	if (address != NULL)
	{
		buffer_len = snprintf(NULL, 0, format, get_log_level(log_level), timestamp, address, port, message);
	}
	else 
	{
		buffer_len = snprintf(NULL, 0, format, get_log_level(log_level), timestamp, message);
	}
	if (buffer_len < 0)
	{
		perror("snprintf length calculation failed");
		free(timestamp);
		return ;
	}
	buffer = (char *)malloc(sizeof(char) * (buffer_len + 1));
	if (!buffer)
	{
		fprintf(stderr, "Failed to log command\n");
		free(buffer);
		free(timestamp);
		return ;
	}
	if (address != NULL)
	{
		written = snprintf(buffer, buffer_len + 1, format, get_log_level(log_level), timestamp, address, port, message);
	}
	else
	{
		written = snprintf(buffer, buffer_len + 1, format, get_log_level(log_level), timestamp, message);
	}
	if (written < 0 || written > buffer_len)
	{
		perror("snprintf formatting failed");
		free(buffer);
		free(timestamp);
		return ;
	}
	pthread_mutex_lock(&log_file_lock);
	if (write(g_log_file, buffer, written) == -1)
	{
		perror("Failed to write log");
	}
	pthread_mutex_unlock(&log_file_lock);
	free(buffer);
	free(timestamp);
}

void    alogf(int log_level, const char *address, uint16_t port, char *message, ...)
{
    char	*timestamp;
    char	format[128];
	char	*buffer;
	int		buffer_len;
	int		written;
	va_list	args;

	va_start(args, message);
	message = stringf(message, args);
	va_end(args);
	if (!message)
	{
		fprintf(stderr, "Failed to format string\n");
		return ;
	}
	if (address != NULL)
	{
		snprintf(format, sizeof(format), "[%%s] [%%s] [%%s:%%d] %%s\n");
	}
	else
	{
		snprintf(format, sizeof(format), "[%%s] [%%s] %%s\n");
	}
    timestamp = get_current_timestamp(time(NULL));
    if (!timestamp)
	{
		fprintf(stderr, "Failed to generate log timestamp\n");
		free(message);
		return ;
	}
	if (address != NULL)
	{
		buffer_len = snprintf(NULL, 0, format, get_log_level(log_level), timestamp, address, port, message);
	}
	else 
	{
		buffer_len = snprintf(NULL, 0, format, get_log_level(log_level), timestamp, message);
	}
	if (buffer_len < 0)
	{
		perror("snprintf length calculation failed");
		free(timestamp);
		free(message);
		return ;
	}
	buffer = (char *)malloc(sizeof(char) * (buffer_len + 1));
	if (!buffer)
	{
		fprintf(stderr, "Failed to log command\n");
		free(buffer);
		free(timestamp);
		free(message);
		return ;
	}
	if (address != NULL)
	{
		written = snprintf(buffer, buffer_len + 1, format, get_log_level(log_level), timestamp, address, port, message);
	}
	else
	{
		written = snprintf(buffer, buffer_len + 1, format, get_log_level(log_level), timestamp, message);
	}
	if (written < 0 || written > buffer_len)
	{
		perror("snprintf formatting failed");
		free(buffer);
		free(timestamp);
		free(message);
		return ;
	}
	pthread_mutex_lock(&log_file_lock);
	if (write(g_log_file, buffer, written) == -1)
	{
		perror("Failed to write log");
	}
	pthread_mutex_unlock(&log_file_lock);
	free(buffer);
	free(timestamp);
	free(message);
}