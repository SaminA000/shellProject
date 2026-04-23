#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define MAX_ARGS 100
#define MAX_PATHS 100

void error() {
fprintf(stderr, "An error has ocurred\n");
exit(1);
}

int main(int argc, char *argv[]) {
char *line = NULL;
size_t len = 0;
FILE *input = stdin;
int interactive = 1;
int last_status = 0;
char *cmd;
char *args[MAX_ARGS];
char *saveptr1, *saveptr2;
int i;


//handling batch
if (argc == 2) {
input = fopen(argv[1], "r");
if (input == NULL) {
fprintf(stderr, "Error: Batch file not found\n");
exit(1);
}
interactive = 0;
} else if (argc > 2) {
fprintf(stderr, "Usage: shellP [batch-file]\n");
exit(1);
}


//Initializing path
char *shell_paths[MAX_PATHS];
shell_paths[0] = strdup("/bin");
shell_paths[1] = NULL;

while (1) {

if (interactive) {
printf("shellP> ");
fflush(stdout);
}

if (getline(&line, &len, input) == -1) break;

int is_parallel = (strchr(line, '&') != NULL);
cmd = strtok_r(line, ";&\n", &saveptr1);

while (cmd != NULL) {
i = 0;
char *token = strtok_r(cmd, " \t\r\n", &saveptr2);
while (token != NULL && i < MAX_ARGS - 1) {
args[i++] = token;
token = strtok_r(NULL, " \t\r\n", &saveptr2);
}
args[i] = NULL;

if (args[0] == NULL) {
cmd = strtok_r(NULL, ";&\n", &saveptr1);
continue;
}


//exit
if (strcmp(args[0], "exit") == 0) {
if (args[1] != NULL) {
fprintf(stderr, "An error has ocurred\n");
cmd = strtok_r(NULL, ";\n", &saveptr1);
continue;
} else {
exit(last_status);
}
}

//cd
if (strcmp(args[0], "cd") == 0) {
if (args[1] == NULL || args[2] != NULL) {
fprintf(stderr, "An error has ocurred\n");
} else {
if (chdir(args[1]) != 0) {
fprintf(stderr, "An error has ocurred\n");
}
}
cmd = strtok_r(NULL, ";&\n", &saveptr1);
continue;
}


//path
if (strcmp(args[0], "path") == 0) {
if (args[1] == NULL) {
for (int j = 0; shell_paths[j] != NULL; j++) {
printf("%s%s", shell_paths[j], (shell_paths[j+1] != NULL) ? " " : "");
}
printf("\n");
last_status = 0;
}
else {
for (int j = 0; shell_paths[j] != NULL; j++) {
free(shell_paths[j]);
}

int j = 0;
while (args[j+1] != NULL && j < MAX_PATHS - 1) {
shell_paths[j] = strdup(args[j+1]);
j++;
}
shell_paths[j] = NULL;
last_status = 0;
}
cmd = strtok_r(NULL, ";&\n", &saveptr1);
continue;
}


//fork
pid_t pid = fork();

if (pid == 0) {
int redirect_count = 0;
for (int k = 0; args[k] != NULL; k++) {
if (strcmp(args[k], ">") == 0) {
redirect_count++;
if (redirect_count >1 || args[k+1] == NULL) error();

int fd = open(args[k+1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
if (fd < 0) error();

dup2(fd, STDOUT_FILENO);
dup2(fd, STDERR_FILENO);
close(fd);
args[k] = NULL;
}
else if (strcmp(args[k],"<") == 0) {
redirect_count++;
if(redirect_count > 1 || args[k+1] == NULL) error();

int fd = open(args[k+1], O_RDONLY);
if (fd < 0) error();

dup2(fd, STDIN_FILENO);
close(fd);
args[k] = NULL;
}
}

if (strchr(args[0], '/') != NULL) {
execv(args[0], args);
} else {
for (int j = 0; shell_paths[j] != NULL; j++) {
char full_path[1024];
snprintf(full_path, sizeof(full_path), "%s/%s", shell_paths[j], args[0]);
if (access(full_path, X_OK) == 0) execv(full_path, args);
}
}
error();
} else if (pid > 0) {
if (!is_parallel) {
int status;
if (waitpid(pid, &status, 0) > 0) {
if (WIFEXITED(status)) {
last_status = WEXITSTATUS(status);
}
}
}
} else {
error();
}
cmd = strtok_r(NULL, ";&\n", &saveptr1);
}

int status;

if (is_parallel) {
int any_fail = 0;
while(wait(&status) > 0) {
if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
any_fail=1;
}
}
last_status = any_fail ? 1 : 0;
} else {
while (wait(&status) > 0);
}
}

free(line);
if (input != stdin) fclose(input);
return last_status;
}
