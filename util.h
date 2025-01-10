#ifndef UTIL_H
#define UTIL_H

void tsh_loop();

char* tsh_read_line();
char** tsh_split_line(const char* line);
int tsh_execute(const char** args);

#endif