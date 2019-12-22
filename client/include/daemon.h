#ifndef DAEMON_H
# define DAEMON_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#define BUFFER_LENGTH 20000
#define PORT 8090
#define IP_SERVER "192.168.44.135"

int send_message(char *message);
int	create_daemon();

# endif