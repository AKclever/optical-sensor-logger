
#define _POSIX_C_SOURCE 200809L

// SPDX-License-Identifier: MIT
// ChatGPT helped with the initial scaffold and the shutdown changes below.

#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "logger.h"
#include "sensor.h"

// Display command-line help.
static void print_usage(const char *program) {
  printf("Usage: %s [--help] [--interval MS] [--output PATH]\n", program);
  printf("\n");
  printf("Record simulated optical sensor data to a CSV file.\n");
  printf("\n");
  printf("Options:\n");
  printf("  --help          Display this help message\n");
  printf("  --interval MS   Set sampling interval in milliseconds\n");
  printf("  --output PATH   Set output CSV filename\n");
}

static volatile sig_atomic_t sensor_pid = -1;

// ChatGPT helped with signal forwarding and the fork-time signal mask.
// kill is async-signal-safe; file cleanup stays in normal program flow.
static void handle_parent_stop(int signal_number) {
  (void)signal_number;
  int saved_errno = errno;
  if (sensor_pid > 0) {
    (void)kill((pid_t)sensor_pid, SIGTERM);
  }
  errno = saved_errno;
}

int main(int argc, char *argv[]) {
  int interval_ms = 1000;
  const char *output_path = "readings.csv";

  // Check options before creating files or processes.
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

      if (errno == ERANGE || *end != '\0' || value <= 0 || value > INT_MAX) {

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
      fprintf(stderr, "Error: Unknown argument: %s\n", argv[i]);
      return EXIT_FAILURE;
    }
  }

  // Keep stop signals pending until both processes are ready to handle them.
  sigset_t stop_signals;
  sigset_t previous_mask;
  sigemptyset(&stop_signals);
  sigaddset(&stop_signals, SIGINT);
  sigaddset(&stop_signals, SIGTERM);
  if (sigprocmask(SIG_BLOCK, &stop_signals, &previous_mask) == -1) {
    perror("block stop signals");
    return EXIT_FAILURE;
  }

  struct sigaction action = {0};

  action.sa_handler = handle_parent_stop;
  action.sa_flags = SA_RESTART;
  sigemptyset(&action.sa_mask);

  if (sigaction(SIGINT, &action, NULL) == -1 ||
      sigaction(SIGTERM, &action, NULL) == -1) {
    perror("sigaction");
    return EXIT_FAILURE;
  }

  int fd[2];

  if (pipe(fd) == -1) {
    perror("pipe");
    return EXIT_FAILURE;
  }

  pid_t pid = fork();

  if (pid == -1) {
    perror("fork");
    close(fd[0]);
    close(fd[1]);
    return EXIT_FAILURE;
  }

  if (pid == 0) {
    // The child only writes to the pipe.
    if (close(fd[0]) == -1) {
      perror("child close unused pipe end");
      _exit(EXIT_FAILURE);
    }

    int result = sensor_run(fd[1], (unsigned int)interval_ms);

    // Close the writing end when finished.
    if (close(fd[1]) == -1) {
      perror("child close");
      result = -1;
    }

    _exit(result == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
  }

  sensor_pid = (sig_atomic_t)pid;
  int logger_result = 0;
  if (sigprocmask(SIG_SETMASK, &previous_mask, NULL) == -1) {
    perror("restore signal mask");
    logger_result = -1;
  }

  // Only the child keeps a writing end, so its exit produces EOF.
  if (close(fd[1]) == -1) {
    perror("parent close unused pipe end");
    logger_result = -1;
  }
  if (logger_result == 0) {
    logger_result = logger_run(fd[0], output_path);
  }
  // Stop forwarding before waitpid can release the child's PID for reuse.
  sensor_pid = -1;

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

  // Always collect the child, including when logging failed.
  int status;
  pid_t waited;

  do {
    waited = waitpid(pid, &status, 0);
  } while (waited == -1 && errno == EINTR);

  if (waited == -1) {
    perror("waitpid");
    return EXIT_FAILURE;
  }

  if (logger_result != 0) {
    fprintf(stderr, "Error: Logger failed\n");
    return EXIT_FAILURE;
  }

  if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS) {

    fprintf(stderr, "Error: Sensor process failed\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
