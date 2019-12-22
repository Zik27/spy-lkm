#include "daemon.h"

int monitoring_chardev()
{
	int chardev_fd;
	char user_buffer[BUFFER_LENGTH];
	int	 ret;

	while (1)
	{
		sleep(30);
		chardev_fd = open("/dev/secret_device", O_RDONLY);
		if (chardev_fd < 0)
			return (-1);
		ret = read(chardev_fd, user_buffer, BUFFER_LENGTH);
		if (ret < 0)
			return (-1);
		else if (!ret)
		{
			close(chardev_fd);
			continue;
		}
		else
		{
			if (!send_message(user_buffer))
			{
				close(chardev_fd);
        		continue;
			}
        	else
        	{
        		printf("ERROR\n");
        		return (-1);
        	}
		}
	}
	return (0);
}

int	create_daemon()
{
	int 	status;
	pid_t	process_id;
	pid_t	sid;

	if ((process_id = fork()) == -1)
	{
		printf("fork failed!\n");
		exit(-1);
	}
	else if (!process_id) // Child process
	{
		umask(0);
		//set new session
		if((sid = setsid()) == -1)
			exit(-1);
		chdir("/");
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);
		status = monitoring_chardev();
		return (status);
	}
	else
	{
		printf("process_id of child process %d\n", process_id);
		exit(0);
	}
}