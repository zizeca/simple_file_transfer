/**
 * @file main.c
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief simple server for file transfer
 * @version 0.1
 * @date 2023-04-13
 *
 * @copyright Copyright (c) 2023
 *
 */
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
#include "logger.h"
#include "child_collector.h"

#define LISTEN_BACKLOG 10

//! global variable
bool gb_trem = false;
pid_t parent_pid;

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa) {
  if (sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }

  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}

// sig handler
void sig_handler(int s) {
  switch (s) {
    case SIGCHLD: {
      // waitpid() might overwrite errno, so we save and restore it:
      int saved_errno = errno;
      pid_t pid;
      while ((pid = waitpid(-1, NULL, WNOHANG)) > 0) {
        child_remove(pid);
      }
      errno = saved_errno;
    } break;
    case SIGTERM:
      gb_trem = true;
      log_write("SIGTERM");
      break;
    case SIGHUP:
      child_kill(SIGHUP);
      log_write("SIGHUP");
      break;
  }
}

int main(int argc, char *argv[]) {
  // parse command line
  struct Args arg;
  arg = command_parse(argc, argv);

  // simple logging. (maybe use syslog)
  FILE *logfile;
  if (arg.mode == CONSOLE_MODE) {
    logfile = stdout;
  }
  else {
    // DAEMON_MODE
    logfile = fopen("file_transfer.log", "a");
  }
  if (logfile == NULL) {
    perror("FILE log");
    exit(1);
  }

  // init logger
  log_init(logfile);

  // init daemon, or raplace fork as in man daemon(7)
  if (arg.mode == DAEMON_MODE && daemon(0, 0) == -1) {
    log_write("daemon %s", strerror(errno));
    exit(1);
  }

  //
  parent_pid = getpid();

  // socket
  int sockfd, new_fd;
  sockfd = open_port(arg.port, LISTEN_BACKLOG);

  // signal handler,  can replace on signal(2)
  struct sigaction sa;
  sa.sa_handler = sig_handler;  // reap all dead processes
  sigemptyset(&sa.sa_mask);
  if (sigaction(SIGCHLD, &sa, NULL) == -1 ||
      sigaction(SIGTERM, &sa, NULL) == -1 ||
      sigaction(SIGHUP, &sa, NULL) == -1) {
    // fprintf(logfile, "sigaction %s\n", strerror(errno));
    log_write("sigaction %s", strerror(errno));
    exit(1);
  }

  // accept connection
  socklen_t sin_size;
  struct sockaddr_storage their_addr;  // connector's address information
  char s[INET6_ADDRSTRLEN];
  log_write("Pid: %d.", parent_pid);
  log_write("start server...");

  while (1) {  // main accept() loop
    sin_size = sizeof their_addr;
    // accept
    new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
    if (new_fd < 0) {
      // interrupt
      if (errno == EINTR) {
        if (gb_trem) {
          child_kill(SIGHUP);
          break;
        }
        continue;
      }
      log_write("accept %s", strerror(errno));
      exit(1);
    }

    inet_ntop(their_addr.ss_family,
              get_in_addr((struct sockaddr *)&their_addr),
              s, sizeof s);

    log_write("connect: %s", s);

    pid_t pid = fork();
    if (pid == 0) {   // this is the child process
      close(sockfd);  // child doesn't need the listener
      child_clean();  // child doesn't need the pids collection

      sa.sa_handler = sig_child_handler;
      sigaction(SIGHUP, &sa, NULL);

      // client process
      client_handler(new_fd);

      close(new_fd);
      exit(0);
    }
    else if (pid > 0) {
      child_add(pid);
    }

    close(new_fd);  // parent doesn't need this
  }

  // close logfile.
  fclose(logfile);
  child_clean();
  return 0;
}