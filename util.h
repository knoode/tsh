#ifndef _UTIL_H
#define _UTIL_H

#define TSH_CMD_BUF_SZ   512
#define TSH_TOKEN_BUF_SZ 64
#define TSH_TOKEN_DELIM  " \t\r\n\a"

void tsh_loop();

char* tsh_read_line();
char** tsh_split_line(char* line);
int tsh_execute(char** args);
int tsh_launch(char** args);

// Shell builtins
int tsh_cd(char** args);
int tsh_help(char** args);
int tsh_exit(char** args);

static const char* builtin_str[] = {
    "cd",
    "help",
    "exit"
};

int tsh_num_bultins();

static int (*builtin_func[])(char**) = {
    &tsh_cd,
    &tsh_help,
    &tsh_exit
};

#endif