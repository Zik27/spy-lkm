#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include <sys/types.h>

#define BUFFER_LENGTH 20000
static char user_buffer[BUFFER_LENGTH];

int main(){
	int ret_val, fd;

	fd = open("/dev/secret_device", O_RDONLY);             // Open the device with read only access

	if (fd < 0){
		perror("Failed to open the device secret_device!");
		return errno;
	}

	ret_val = read(fd, user_buffer, BUFFER_LENGTH);        // Read the response

	if (ret_val < 0){
		perror("Failed to read the message from the secret_device");
		return errno;
	}

	printf("%s", user_buffer);			// display the buffer contents
	close(fd);
	return 0;
}
