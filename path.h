#pragma once

#include <stdio.h>

void type_command(char *cmd[]);
char *pwd(char logical_pwd[]);
char *find_executable(char *cmd);
int handle_redirection(char *args[], int *redirection_fd);
void format_input(char *agrs[], char cmd[]);
char **my_completion(const char *text, int start, int end);
void custom_echo(char *args[]);
void custom_cd(char *args[], char logical_pwd[]);
void custom_executable(char *args[]);
void run_pipeline(char **cmd1, char **cmd2);
void parse_command(char *input, char **args);
char **command_completion(const char *text);
int is_builtin(char *cmd);
void run_multi_pipeline(char ***commands, int n);
void handle_sigint();
void custom_history(char *args[], char *input);
void custom_append_history();
void sync_history_append_index();
