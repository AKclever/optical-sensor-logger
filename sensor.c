
#define _POSIX_C_SOURCE 200809L

// SPDX-License-Identifier: MIT
// Initial scaffold created with ChatGPT assistance.
#include "sensor.h"

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

// Flag used to stop the sensor safely.
static volatile sig_atomic_t stop_requested = 0;

// Called when Ctrl+C or SIGTERM is received.
static void handle_stop(int signal_number) {
  (void)signal_number;
  stop_requested = 1;
}

int sensor_run(int output_fd, unsigned int interval_ms) {
  // Install signal handlers.
  struct sigaction action = {0};
  action.sa_handler = handle_stop;
  sigemptyset(&action.sa_mask);

  if (sigaction(SIGINT, &action, NULL) == -1 ||
      sigaction(SIGTERM, &action, NULL) == -1) {
    perror("sigaction");
    return -1;
  }

  // Prevent SIGPIPE from terminating the child.
  struct sigaction ignore_pipe = {0};
  ignore_pipe.sa_handler = SIG_IGN;
  sigemptyset(&ignore_pipe.sa_mask);

  if (sigaction(SIGPIPE, &ignore_pipe, NULL) == -1) {
    perror("sigaction SIGPIPE");
    return -1;
  }

  // ChatGPT helped close the startup race: main blocks these before fork,
  // and the child unblocks them only after its own handlers are installed.
  sigset_t stop_signals;
  sigemptyset(&stop_signals);
  sigaddset(&stop_signals, SIGINT);
  sigaddset(&stop_signals, SIGTERM);
  if (sigprocmask(SIG_UNBLOCK, &stop_signals, NULL) == -1) {
    perror("unblock sensor stop signals");
    return -1;
  }

  // Convert milliseconds into seconds and nanoseconds.
  struct timespec delay = {.tv_sec = interval_ms / 1000,
                           .tv_nsec = (long)(interval_ms % 1000) * 1000000L};

  unsigned int sample_number = 0;

  // Keep generating readings until shutdown.
  while (!stop_requested) {

    // Get the current Unix timestamp.
    time_t timestamp = time(NULL);

    if (timestamp == (time_t)-1) {
      perror("time");
      return -1;
    }

    // Generate synthetic optical intensity.
    double intensity = 42.5 + (sample_number % 10) * 0.1;

    // Format the CSV record.
    char buffer[128];

    int length = snprintf(buffer, sizeof(buffer), "%lld,%.2f\n",
                          (long long)timestamp, intensity);

    if (length < 0 || (size_t)length >= sizeof(buffer)) {
      fprintf(stderr, "Error formatting sensor reading\n");
      return -1;
    }

    // Write the complete record to the pipe.
    size_t total = 0;

    while (total < (size_t)length) {
      ssize_t written =
          write(output_fd, buffer + total, (size_t)length - total);

      if (written == -1) {
        if (errno == EINTR) {
          // Finish an interrupted write if possible.
          continue;
        }

        perror("sensor write");
        return -1;
      }

      if (written == 0) {
        fprintf(stderr, "Sensor write made no progress\n");
        return -1;
      }

      total += (size_t)written;
    }

    sample_number++;

    // Wait until the next sample.
    struct timespec remaining = delay;

    while (!stop_requested && nanosleep(&remaining, &remaining) == -1) {
      if (errno != EINTR) {
        perror("nanosleep");
        return -1;
      }
    }
  }

  return 0;
}
