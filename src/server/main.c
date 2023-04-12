
#define _GNU_SOURCE

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "command_parse.h"
#include "open_port.h"
#include "client_handler.h"

#define LISTEN_BACKLOG 50

//! global variable
bool gb_trem = false;
bool gb_hup = false;
FILE* logfile;


// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// sig handler
void sigchld_handler(int s) {
  switch (s) {
    case SIGCHLD:
      // waitpid() might overwrite errno, so we save and restore it:
      int saved_errno = errno;
      while (waitpid(-1, NULL, WNOHANG) > 0)
        ;
      errno = saved_errno;
      break;
    case SIGTERM:
      gb_trem = true;
      printf("signal terminate\n");
      break;
    case SIGHUP:
      gb_hup = true;
      printf("signal hup\n");
      break;
  }
}

int main(int argc, char *argv[]) {
  // parse command line
  struct Args arg;
  arg = ParseCommand(argc, argv);

  // simple logging. (maybe use syslog) 
  if(arg.mode == CONSOLE_MODE) {
    logfile = stdout;
  } else {
    // DAEMON_MODE
    logfile = fopen("file_transfer.log", "a");
    fprintf(logfile, "start log \n");
    setbuf(logfile, NULL);
  }
  if(logfile == NULL) {
    perror("FILE log");
    exit(1);
  }

  // init daemon, or raplace fork as in man daemon(7)
  if(arg.mode == DAEMON_MODE && daemon(0, 0) == -1) {
    fprintf(logfile, "daemon %s\n", strerror(errno));
    exit(1);
  }

  // socket
  int sockfd, new_fd;
  sockfd = open_port(arg.port, 10);

  // signal handler,  can replace on signal(2)
  struct sigaction sa;
  sa.sa_handler = sigchld_handler;  // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1 ||
      sigaction(SIGTERM, &sa, NULL) == -1 ||
      sigaction(SIGHUP, &sa, NULL) == -1) {
    fprintf(logfile, "sigaction %s\n", strerror(errno));
    exit(1);
  }

  socklen_t sin_size;
  struct sockaddr_storage their_addr;  // connector's address information
  char s[INET6_ADDRSTRLEN];
  fprintf(logfile, "Pid: %d.\n", getpid());
  fprintf(logfile, "start server...\n");

  while (1) {  // main accept() loop
    sin_size = sizeof their_addr;
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd == -1) {
      fprintf(logfile, "accept %s\n", strerror(errno));
      continue;
    }


    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *)&their_addr),
              s, sizeof s);
    
    fprintf(logfile, "server: got connection from %s\n", s);

    if (!fork()) {    // this is the child process
      close(sockfd);  // child doesn't need the listener
      // if (send(new_fd, "Hello, world!", 13, 0) == -1)
      //   perror("send");
      client_handler(new_fd);
      close(new_fd);
      exit(0);
    }
    close(new_fd);  // parent doesn't need this
  }


  fprintf(logfile, "end programm\n");
  return 0;
}