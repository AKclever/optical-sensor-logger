// SPDX-License-Identifier: MIT
// Starter scaffold created with ChatGPT assistance; TODO logic is left to the student.
#include <stdio.h>
#include <stdlib.h>

#include "logger.h"
#include "sensor.h"

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    // TODO 1: Parse --help, --interval (milliseconds), and --output.
    // TODO 2: Create a pipe, then fork a child process.
    // TODO 3: Close unused pipe ends in BOTH processes.
    // TODO 4: Child calls sensor_run; parent calls logger_run.
    // TODO 5: Handle SIGINT/SIGTERM and collect the child with waitpid.
    // TODO 6: Clean up on every error path and return a suitable exit status.
    fprintf(stderr, "Not implemented yet. Start with TODO 1 in main.c.\n");
    return EXIT_FAILURE;
}
