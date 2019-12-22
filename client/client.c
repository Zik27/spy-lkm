#include "daemon.h"

int send_message(char *message)
{
    int socket_fd;
    struct sockaddr_in addr;
    int     data_size;
    int     bytes_sent;

    data_size = strlen(message);
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        printf("Socket creation error\n");
        exit (-1);
    }
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP_SERVER);
    if(connect(socket_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        printf("Connect failed\n");
        exit(-1);
    }
    while (data_size > 0)
    {
        bytes_sent = send(socket_fd, message, data_size, 0);
        if (bytes_sent == SO_ERROR)
        {
            printf("ERROR SEND\n");
            return -1;
        }
        message += bytes_sent;
        data_size -= bytes_sent;
    }
    close(socket_fd);
    return (0);
}
