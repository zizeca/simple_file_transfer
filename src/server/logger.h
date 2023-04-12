/**
 * @file logger.h
 * @author Enver Kulametov (zizu.meridian@gmail.com)
 * @brief Simple logger
 * @version 0.1
 * @date 2023-04-12
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

// GLOBAL VARIABLE
static FILE* _log_file;

void log_init(FILE* file) {
  if (file == NULL) {
    _log_file = stdout;
    return;
  }

  _log_file = file;
}

void log_write(const char* format, ...) {
  // char buf[20];
  struct tm * loc_time;
  time_t timer = time(NULL);
  loc_time = localtime(&timer);
  // strftime(buf, sizeof buf, "%H:%M:%S", loc_time);
  fprintf(_log_file, "[%02d:%02d:%02d] ", loc_time->tm_hour, loc_time->tm_min, loc_time->tm_sec);

  va_list arglist;
  va_start(arglist, format);
  vfprintf(_log_file, format, arglist);
  va_end(arglist);
  fputc('\n', _log_file);
  fflush(_log_file);
}

// void log_close() {
//   if (_log_file != NULL) {
//     fclose(_log_file);
//   }
// }

#endif  // __LOGGER_H__