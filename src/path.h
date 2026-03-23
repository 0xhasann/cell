#pragma once

#include <stdio.h>

void type_command(char *cmd[]);
char *pwd();
char *find_executable(char *cmd);
int handle_redirection(char *args[], int *redirection_fd);
