
#define _GNU_SOURCE

#include <arpa/inet.h>
#include <getopt.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>


#include "command_parse.h"

int main(int argc, char *argv[]) {
  struct Args arg;
  arg = ParseCommand(argc, argv);

  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  struct addrinfo *res, *p;

  
  printf("start server\n");

  printf("end programm\n");
  return 0;
}