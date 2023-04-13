/**
 * @file main.c
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief simple client for file transfer
 * @version 0.1
 * @date 2023-04-11
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#define _GNU_SOURCE

#include <unistd.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "create_connection.h"

#define PORT "5000"  // the port client will be connecting to

#define MAXDATASIZE BUFSIZ  // max number of bytes we can get at once



int main(int argc, char *argv[]) {
  int numbytes, retval;
  int sockfd;
  char buf[MAXDATASIZE];


  if (argc != 4) {
    fprintf(stderr, "client <ip> <port> <file>\n");
    exit(1);
  }

  const char* server_ip = argv[1];
  const char* server_port = argv[2];
  const char* file_path = argv[3]; // aliase

  FILE* file = fopen(file_path, "r");
  if(file == NULL) {
    printf("Fail to read file%s\n", file_path);
    exit(EXIT_FAILURE);
  }

  sockfd = create_connection(server_ip, server_port);
  if(sockfd == -1) {
    printf("Fail to create connection ip=%s, port=%s. Error: %s\n", 
            server_ip, 
            server_port, 
            strerror(errno));
    exit(1);
  }

  const char *file_name = basename(file_path);
  struct stat file_stat;
  retval = stat(file_path, &file_stat);
  if(retval == -1) {
    perror("stat");
    exit(1);
  }

  // encode message
  // send file name and file size
  sprintf(buf, "%d %s", file_stat.st_size, file_name);
  numbytes = send(sockfd, buf, strlen(buf) + 1, 0); // send file name
  if(numbytes == -1) {
    perror("send");
    exit(1);
  }

  numbytes = recv(sockfd, buf, MAXDATASIZE, 0);
  if(numbytes == -1) {
    perror("recv after send file name");
    exit(1);
  }

  int retcode = 0;
  char msg[255];
  printf("response: %.*s\n", numbytes, buf);

  int fd = fileno(file);
  retval = sendfile(sockfd, fd, NULL, BUFSIZ);
  if(retval == -1) {
    perror("sendfile");
    fclose(file);
    exit(1);
  }


  close(sockfd);
  fclose(file);

  return 0;
}
