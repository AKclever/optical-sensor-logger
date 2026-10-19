
#define _POSIX_C_SOURCE 200809L

// SPDX-License-Identifier: MIT
// Starter scaffold created with ChatGPT assistance.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "logger.h"
#include "sensor.h"

// Display command-line help.
static void print_usage(const char *program)
{
    printf("Usage: %s [--help] [--interval MS] [--output PATH]\n", program);
    printf("\n");
    printf("Record simulated optical sensor data to a CSV file.\n");
    printf("\n");
    printf("Options:\n");
    printf("  --help          Display this help message\n");
    printf("  --interval MS   Set sampling interval in milliseconds\n");
    printf("  --output PATH   Set output CSV filename\n");
}

// The parent stays alive when Ctrl+C is pressed.
// The child handles stopping its sensor loop.
static void handle_parent_sigint(int signal_number)
{
    (void)signal_number;
}

int main(int argc, char *argv[])
{
    int interval_ms = 1000;
    const char *output_path = "readings.csv";

    // TODO 1: Parse command-line arguments.
    for (int i = 1; i < argc; i++) {

        if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return EXIT_SUCCESS;
        }

        else if (strcmp(argv[i], "--interval") == 0) {

            if (i + 1 >= argc) {
                fprintf(stderr, "Error: Missing interval value\n");
                return EXIT_FAILURE;
            }

            char *end;
            errno = 0;

            long value = strtol(argv[++i], &end, 10);

            if (errno == ERANGE ||
                *end != '\0' ||
                value <= 0 ||
                value > INT_MAX) {

                fprintf(stderr, "Error: Invalid interval\n");
                return EXIT_FAILURE;
            }

            interval_ms = (int)value;
        }

        else if (strcmp(argv[i], "--output") == 0) {

            if (i + 1 >= argc) {
                fprintf(stderr, "Error: Missing output filename\n");
                return EXIT_FAILURE;
            }

            output_path = argv[++i];

            if (output_path[0] == '\0') {
                fprintf(stderr, "Error: Empty output filename\n");
                return EXIT_FAILURE;
            }
        }

        else {
            fprintf(stderr, "Error: Unknown argument: %s\n",
                    argv[i]);
            return EXIT_FAILURE;
        }
    }

    // TODO 5: Configure Ctrl+C handling for the parent.
    struct sigaction action = {0};

    action.sa_handler = handle_parent_sigint;
    action.sa_flags = SA_RESTART;
    sigemptyset(&action.sa_mask);

    if (sigaction(SIGINT, &action, NULL) == -1) {
        perror("sigaction");
        return EXIT_FAILURE;
    }

    // TODO 2: Create a pipe.
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    // TODO 2: Create a child process.
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        close(fd[0]);
        close(fd[1]);
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        // CHILD PROCESS: Sensor simulator

        // TODO 3: Child does not read from the pipe.
        close(fd[0]);

        // TODO 4: Generate readings and send them.
        int result = sensor_run(fd[1],
                                (unsigned int)interval_ms);

        // Close the writing end when finished.
        if (close(fd[1]) == -1) {
            perror("child close");
            result = -1;
        }

        _exit(result == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
    }

    // PARENT PROCESS: CSV logger

    // TODO 3: Parent does not write to the pipe.
    close(fd[1]);

    // TODO 4: Receive readings and save them.
    int logger_result = logger_run(fd[0], output_path);

    // Close the reading end.
    if (close(fd[0]) == -1) {
        perror("parent close");
        logger_result = -1;
    }

    // Stop the child if logging failed.
    if (logger_result != 0) {
        if (kill(pid, SIGTERM) == -1 && errno != ESRCH) {
            perror("kill");
        }
    }

    // TODO 5: Wait for the child to exit.
    int status;
    pid_t waited;

    do {
        waited = waitpid(pid, &status, 0);
    } while (waited == -1 && errno == EINTR);

    if (waited == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    // TODO 6: Check process results.
    if (logger_result != 0) {
        fprintf(stderr, "Error: Logger failed\n");
        return EXIT_FAILURE;
    }

    if (!WIFEXITED(status) ||
        WEXITSTATUS(status) != EXIT_SUCCESS) {

        fprintf(stderr, "Error: Sensor process failed\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
