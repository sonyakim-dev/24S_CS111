#include <fcntl.h>
#include <stdio.h> // for fprintf, perror
#include <stdlib.h> // for exit
#include <unistd.h> // for fork, dup2
#include <sys/wait.h> // for waitpid
#include <errno.h>

int main(int argc, char *argv[])
{
    // 0 arg => exit
    if (argc <= 1) {
        fprintf(stderr, "Usage: %s [COMMANDS]\n", argv[0]);
        exit(EINVAL);
    }
    // 1 arg => execute the command
    if (argc == 2) {
        execlp(argv[1], argv[1], NULL);
        perror("ERR: execlp failed\n");
        exit(errno);
    }

    int pipefd[2]; // store read and write file descriptors
    // *NOTE* pipefd[0]: read, pipefd[1]: write

    // create n child processes
    for (int i = 1; i < argc; ++i) {
        // pipe n-1 times for n commands
        if (i < argc - 1) {
            if (pipe(pipefd) == -1) {
                perror("ERR: pipe failed\n");
                exit(errno);
            }
        }

        pid_t cpid = fork(); // create a new process

        if (cpid == -1) {
            perror("ERR: fork failed\n");
            exit(errno);
        }

        // CHILD PROCESS
        if (cpid == 0) {
            // not the last command
            if (i < argc - 1) {
                dup2(pipefd[1], STDOUT_FILENO); // redirect stdout to pipe write
            }
            close(pipefd[0]); // close pipe read
            close(pipefd[1]); // close pipe write

            execlp(argv[i], argv[i], NULL);

            perror("ERR: execlp failed\n");
            exit(errno);
        }

        // PARENT PROCESS
        else {
            // wait for child process to finish
            int status;
            waitpid(cpid, &status, 0);

            if (!WIFEXITED(status)) {
                fprintf(stderr, "ERR: child process %d exited abnormally\n", i);
                exit(WEXITSTATUS(status));
            }
            if (WEXITSTATUS(status) != 0) {
                fprintf(stderr, "ERR: child process %d exited with status %d\n", i, WEXITSTATUS(status));
                exit(WEXITSTATUS(status));
            }

            // not the last command
            if (i < argc - 1) {
                dup2(pipefd[0], STDIN_FILENO); // redirect stdin to pipe read
            }
            close(pipefd[0]); // close pipe read
            close(pipefd[1]); // close pipe write
        }
    }

	return 0;
}
