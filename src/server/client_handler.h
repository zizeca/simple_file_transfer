#ifndef __CLIENT_HANDLER_H__
#define __CLIENT_HANDLER_H__

#include <sys/socket.h>

#include <stdio.h>
#include <string.h>

void client_handler(int sd) {
  FILE* file;

  char file_name[255];
  int file_size = 0;
  memset(file_name, 0, 255);

  char buf[BUFSIZ];
  memset(buf, 0, BUFSIZ);

  int size = recv(sd, buf, BUFSIZ, 0);

  if(size == -1) {
    perror("recv");
    return;
  }

  int ret = sscanf(buf,"%d %s", &file_size, file_name);
  if(ret == EOF) {
    send(sd, "Fail parsing data", 18, 0);
    printf("scanf data error");
    return;
  }

  printf("file name =%s, file size = %d\n", file_name, file_size);

  /*
  file = fopen(file_name, "w");
  if(file == NULL) {
    send(sd, "Fail to create file", 20, 0);
    return;
  } else {
    send(sd, "Create new file", 16, 0);
  }


  while(true) {
      size = recv(sd, buf, BUFSIZ, 0);
      if(size == 0) break;
      if(size == -1) {
        perror("recv file");
        fclose(file);
        // delete file;
        remove(file_name);
        return;
      }
      fwrite(buf, sizeof(char), BUFSIZ, file);
  }

  fclose(file);
  */
}


#endif // __CLIENT_HANDLER_H__