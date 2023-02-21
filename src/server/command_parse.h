/**
 * @file command_parse.h
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief
 * @version 0.1
 * @date 2023-02-20
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __COMMAND_PARSE_H__
#define __COMMAND_PARSE_H__

#include <getopt.h>

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

#define DEFAULT_PORT 5000

enum Mode {
  DBUG,
  RELEASE
};

struct Args {
  enum Mode mode;
  int port;
};

struct Args ParseCommand(int argc, char *argv[]) {
  struct Args retArg;
  retArg.mode = DBUG;
  retArg.port = DEFAULT_PORT;

  int optchar = 0;
  int optindex = 0;

  const char version[] = "0";

  struct option opts[] = {{"debug", no_argument, 0, 'd'},
                          {"version", no_argument, 0, 'v'},
                          {"help", no_argument, 0, 'h'},
                          {"port", required_argument, 0, 'p'},
                          {NULL, 0, 0, 0}};

  while (-1 != (optchar = getopt_long(argc, argv, "dvhp:", opts, &optindex))) {
    switch (optchar) {
      case 'v':
        printf("version %s\n", version);
        exit(EXIT_SUCCESS);
        break;
      case 'd':
        printf("Debug mode..\n");
        retArg.mode = DBUG;
        break;
      case 'h':
        printf("Help message ....\n");
        // todo for help
        exit(EXIT_SUCCESS);
        break;
      case 'p':
        retArg.port = atoi(optarg);
        if(retArg.port < 1024 || retArg.port > USHRT_MAX) {
          retArg.port = DEFAULT_PORT;
        }

        printf("set port for connection %d\n", retArg.port);
        break;
      default:
        printf("Unknown argument, please add -h for help.");
        exit(EXIT_SUCCESS);
        break;
    }
  }

  return retArg;
}

#endif  // __COMMAND_PARSE_H__