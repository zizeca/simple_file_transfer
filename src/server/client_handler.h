#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include <sys/socket.h>

#include <stdio.h>
#include <string.h>

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
    perror("recv");
    return -1;
  }

  int ret = sscanf(buf, "%d %s", &file_size, file_name);
  if (ret == EOF) {
    send(sd, "Fail parsing data", 18, 0);
    printf("scanf data error");
    return -1;
  }

  printf("file name =%s, file size = %d\n", file_name, file_size);

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
      perror("response");
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
    if (size == -1) {
      perror("recv file");
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