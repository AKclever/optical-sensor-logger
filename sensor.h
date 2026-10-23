// SPDX-License-Identifier: MIT
// Starter scaffold created with ChatGPT assistance.
#ifndef OPTICAL_SENSOR_H
#define OPTICAL_SENSOR_H

// Write newline-terminated timestamp,intensity records (Unix seconds).
// Installs stop handlers and unblocks SIGINT/SIGTERM inherited from main.
// interval_ms must be positive; intensity uses arbitrary simulation units.
// The caller owns output_fd and closes it after this function returns.
// Return 0 for normal completion, -1 for an error.
int sensor_run(int output_fd, unsigned int interval_ms);

#endif
