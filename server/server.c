#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
#define PORT 8090
#define BUF_LEN 20000
#define NAME_FILE "Secret_info.txt"

FILE *get_fp(char *file_name)
{
	int fd;

	FILE *fp;
	fp  = fopen(file_name, "a");
    return (fp);
}

int		write_to_file(char *file_name, char *text, char *ip, int port)
{
	FILE		*fp;
	time_t		t;
    
	time(&t);
	fp = get_fp(file_name);
	fprintf(fp, "\n\n%sIP address: %s\nPort: %d\nMessage:\n%s", ctime(&t), ip, port, text);
	if (fclose (fp) == -1)
	{
		printf("Error close\n");
		exit(-1);
	}
	return (1);
}

int     main(int argc, char **argv)
{
        int                     server_fd;
        struct sockaddr_in      address;
        struct sockaddr_in		clt_addr;
        int						conn_fd;
        char					*buf;
        int						bytes_read;
        int						addrlen;
        int						await;

        addrlen = sizeof(clt_addr);
        await = 1;
		if (!(buf = (char *)malloc(sizeof(*buf) * (BUF_LEN + 1))))
            exit (-1);
        //bzero(buf, BUF_LEN + 1);
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
        {
            printf("Socket creation error\n");
            exit (-1);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = htonl(INADDR_ANY);
        address.sin_port = htons(PORT);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)))
        {
            printf("Bind failed\n");
            exit (-1);
        }
        if (listen(server_fd, 1))
        {
            printf("Listen failed\n");
            exit (-1);
        }
        while(await)
        {
			if ((conn_fd = accept(server_fd, (struct sockaddr*)&clt_addr,  &addrlen)) == -1)
			{
				printf("Accept failed\n");
				exit (-1);
			}
			while(1)
			{
				bytes_read = recv(conn_fd, buf, BUF_LEN, 0);
            	if (bytes_read <= 0) 
					break;
			}
			printf("The message received and written to the file: "NAME_FILE"\n");
            write_to_file(NAME_FILE, buf, inet_ntoa(clt_addr.sin_addr), (int)ntohs(clt_addr.sin_port));
			close(conn_fd);            
       	}
	return(0);
}


