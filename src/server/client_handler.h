#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <stdio.h>
#include <string.h>

#include "logger.h"

#define OUTPUT_DIR "output"

bool child_cancel = false;

void sig_child_handler(int s) {
  child_cancel = true;
}

int client_handler(int sd) {
  FILE* file;
  int retval;
  char file_name[255];
  long int file_size = 0;
  memset(file_name, 0, 255);

  char buf[BUFSIZ];
  memset(buf, 0, BUFSIZ);

  int size = recv(sd, buf, BUFSIZ, 0);

  if (size == -1) {
    log_write("recv: %s", strerror(errno));
    return -1;
  }

  // create directory
  {
  struct stat st = {0};
  if (stat(OUTPUT_DIR, &st) == -1) {
      mkdir(OUTPUT_DIR, 0700);
  }
  }

  // get info from server
  retval = sscanf(buf, "%ld%254s", &file_size, file_name);
  if (retval == EOF) {
    send(sd, "Fail parsing data", 18, 0);
    log_write("scanf data error");
    return -1;
  }

  log_write("file name = %s, file size = %ld\n", file_name, file_size);

  // prifix dir (& swap)
  strcpy(buf, OUTPUT_DIR);
  strcat(buf, "/");
  strcat(buf, file_name);
  strcpy(file_name, buf);

  // work with file
  file = fopen(file_name, "w");
  if (file == NULL) {
    sprintf(buf, "Fail to create file %s (%s)", file_name, strerror(errno));
    log_write("%50s", buf);
    send(sd, buf, strlen(buf) + 1, 0);
    return -1;
  }
  else {
    sprintf(buf, "Create new file %s", file_name);
    log_write("%50s", buf);
    retval = send(sd, buf, strlen(buf) + 1, 0);
    if (retval == -1) {
      log_write("response: %s", strerror(errno));
      fclose(file);
      return -1;
    }
  }

  int byte_left = file_size;

  // loop for recive file
  while (true) {
    size = recv(sd, buf, BUFSIZ, 0);
    if (size == 0) {
      break;
    }

    // error recv
    if (size == -1 || child_cancel) {
      log_write("Error receiving the file: %s", strerror(errno));
      log_write("The incomplete \'%s\' file will be deleted.", file_name);
      fclose(file);
      // delete file;
      remove(file_name);
      return -1;
    }

    fwrite(buf, sizeof(char), size, file);
    byte_left -= size;
    // sleep(1); // uncomment for test bigfile
  }

  if (byte_left != 0) {
    log_write("File \'%s\' download not complited (%s)", file_name, strerror(errno));
    fclose(file);
    remove(file_name);
    return 0;
  } else {
    log_write("The file \'%s\' is written to disk.", file_name);
  }

  fclose(file);
  return 0;
}

#endif  // __CLIENT_HANDLER_H__