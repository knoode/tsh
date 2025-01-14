#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "util.h"

void tsh_loop()
{
    char* line;
    char** args;
    int status;

    do {
        printf("$ ");
        line = tsh_read_line();
        args = tsh_split_line(line);
        status = tsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

char* tsh_read_line()
{
    char* buf = malloc(sizeof(char) * TSH_CMD_BUF_SZ);
    int buf_sz = TSH_CMD_BUF_SZ;
    int pos = 0;
    
    if (!buf) {
        perror("allocation error");
        exit(EXIT_FAILURE);
    }

    while (1) {
        char c = getchar();
        
        if (c == '\n') {
            buf[pos] = '\0';
            return buf;
        } else if (c == EOF) {
            exit(EXIT_SUCCESS);
        } else {
            buf[pos] = c;
        }
        pos++;

        if (pos >= buf_sz) {
            // Reallocate memory
            buf_sz += TSH_CMD_BUF_SZ;
            buf = realloc(buf, sizeof(char) * buf_sz);
            if (!buf) {
                perror("allocation error");
                exit(EXIT_FAILURE);
            }
        }
    }
}

char** tsh_split_line(char* line) 
{
    char* token;
    int position = 0;
    int token_buf_sz = TSH_TOKEN_BUF_SZ;
    char** tokens = malloc(TSH_TOKEN_BUF_SZ * sizeof(char*));

    if (!tokens) {
        // TODO: Maybe make a macro for error-printing
        perror("allocation error");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, TSH_TOKEN_DELIM);
    while (token) {
        tokens[position] = token;
        position++;

        if (position >= token_buf_sz) {
            // Reallocate memory
            token_buf_sz += TSH_TOKEN_BUF_SZ;
            tokens = realloc(tokens, token_buf_sz * sizeof(char*));
            if (!tokens) {
                perror("allocation error");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, TSH_TOKEN_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}

int tsh_execute(char** args)
{
    if (args[0] == NULL) {
        // An empty command was entered
        return 1;
    }

    // Check if shell builtin was entered
    for (int i = 0; i != tsh_num_bultins(); i++) {
        if (strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return tsh_launch(args);
}

int tsh_cd(char** args)
{
    if (args[1] == NULL) {
        perror("tsh");
    } else {
        if (chdir(args[1]) != 0) {
            perror("\"cd\" returned -1");
        }
    } 

    return 1;
}

int tsh_help(char** args)
{
    puts("Test Shell v0.1 (BETA)");
    puts("Coder: kNode");

    puts("Following is the list of builtin commands:");
    for (int i = 0; i != tsh_num_bultins(); i++) {
        printf("   %s\n", builtin_str[i]);
    }

    return 1;
}

int tsh_exit(char** args)
{
    return 0;
}

int tsh_launch(char** args)
{
    pid_t pid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        if (execvp(args[0], args) == -1) {
            perror("tsh");
            exit(EXIT_FAILURE);
        }
    } else if (pid < 0) {
        // Parent process
        perror("tsh");
    } else {
        // Wait until child process dies
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int tsh_num_bultins()
{
    return sizeof(builtin_str) / sizeof(char*);
};