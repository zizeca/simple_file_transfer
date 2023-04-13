#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include <sys/socket.h>

#include <stdio.h>
#include <string.h>

#include "logger.h"

bool child_cancel = false;

void sig_child_handler(int s) {
  child_cancel = true;
}

int fake_handler(int sd, bool *cancel) {
  while(!child_cancel) {
    sleep(2);
    log_write("child");
  }
  log_write("child cancel");
}

int client_handler(int sd) {
  FILE* file;
  int retval;
  char file_name[255];
  int file_size = 0;
  memset(file_name, 0, 255);

  char buf[BUFSIZ];
  memset(buf, 0, BUFSIZ);

  int size = recv(sd, buf, BUFSIZ, 0);

  if (size == -1) {
    log_write("recv: %s", strerror(errno));
    return -1;
  }

  retval = sscanf(buf, "%d %s", &file_size, file_name);
  if (retval == EOF) {
    send(sd, "Fail parsing data", 18, 0);
    log_write("scanf data error");
    return -1;
  }

  log_write("file name = %s, file size = %d\n", file_name, file_size);

  // work with file
  file = fopen(file_name, "w");
  if (file == NULL) {
    sprintf(buf, "Fail to create file %s", file_name);
    send(sd, buf, strlen(buf) + 1, 0);
    return -1;
  }
  else {
    sprintf(buf, "Create new file %s", file_name);
    retval = send(sd, buf, strlen(buf) + 1, 0);
    if (retval == -1) {
      log_write("response: %s", strerror(errno));
      fclose(file);
      return -1;
    }
  }

  // loop for recive file
  while (true) {
    size = recv(sd, buf, BUFSIZ, 0);
    if (size == 0) {
      break;
    }

    // error recv
    if (size == -1 || child_cancel) {
      log_write("recv file: %s", strerror(errno));
      fclose(file);
      // delete file;
      remove(file_name);
      return -1;
    }
    
    fwrite(buf, sizeof(char), size, file);
  }

  fclose(file);
  return 0;
}

#endif  // __CLIENT_HANDLER_H__