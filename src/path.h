#pragma once

#include <stdio.h>

void type_command(char *cmd[]);
char *pwd();
char *find_executable(char *cmd);
int handle_redirection(char *args[], int *redirection_fd);
void format_input(char *agrs[], char cmd[]);
char **my_completion(const char *text, int start, int end);
void custom_echo(char *args[]);
void custom_cd(char *args[]);
void custom_executable(char *args[]);
