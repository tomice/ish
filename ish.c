#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pwd.h>

#define MAX_CMD_LENGTH 100
#define MAX_ARGS 10

static int help(char **args)
{
    printf("************** ICE SHELL **************\n");
    printf("* Only built-in right now is cd\n");
    printf("* Type \"exit\" to leave\n");
    printf("***************************************\n");

    return 1;
}

static int cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "Supply argument to cd\n");
    }
    else {
        if (chdir(args[1]) != 0) {
            perror("chdir");
        }
    }

    return 1;
}

static void parseSpace(char *cmd, char **args)
{
    int i;

    for (i = 0; i < MAX_ARGS; i++) {
        args[i] = strsep(&cmd, " ");
        if (args[i] == NULL) { break; }
        if (strlen(args[i]) == 0) { i--; };
    }
}

static int execute(char **args)
{
    int status;
    pid_t childPid;

    switch (childPid = fork()) {
    case -1: /* Error */
        return -1;
    case 0: /* Child executes command */
        if (execvp(args[0], args) < 0) {
            perror("execvp");
        }
        _exit(127); /* Reserved shell exit code */
    default: /* Parent relays data */
        if (waitpid(childPid, &status, 0) == -1) {
            return -1;
        }
    }

    return 0;
}

static const char *getUsername(void)
{
    uid_t uid = geteuid();
    struct passwd *pw = getpwuid(uid);

    if (pw) {
        return pw->pw_name;
    }

    return "?";
}

static void *getHostname(void)
{
    char hostname[50];

    if (gethostname(hostname, sizeof(hostname)) == 0) {
        printf("%s", hostname);
    }
    else {
        perror("gethostname");
    }

    return 0;
}

static void *getDir(void)
{
    char dir[1024];

    if (getcwd(dir, sizeof(dir)) != NULL) {
        printf("%s", dir);
    }
    else {
        perror("getcwd");
    }
    
    return 0;
}

int main(int argc, char *argv[])
{
    char cmd[MAX_CMD_LENGTH + 1];
    char *args[MAX_ARGS + 1];

    for (;;) {
        /* FIXME: Hack. Fix this, it's embarrassing */
        printf("[%s@", getUsername());
        getHostname();
        printf(" ");
        getDir();
        printf("]$ ");

        if (fgets(cmd, sizeof(cmd), stdin) == NULL) { break; }
        if (cmd[strlen(cmd) - 1] == '\n') {
            cmd[strlen(cmd) - 1] = '\0';
        }
        parseSpace(cmd, args);
        if (strcmp(args[0], "exit") == 0) { break; }
        if (strcmp(args[0], "help") == 0) { help(args); }
        else if (strcmp(args[0], "cd") == 0) { cd(args); }
        else if (execute(args) == 0) { break; }
    }

    return 0;
}
