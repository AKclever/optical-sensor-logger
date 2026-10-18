// SPDX-License-Identifier: MIT
// Starter scaffold created with ChatGPT assistance.
#ifndef OPTICAL_SENSOR_H
#define OPTICAL_SENSOR_H

// Planned contract: write newline-terminated timestamp,intensity records.
// interval_ms must be positive; intensity uses arbitrary simulation units.
// The caller owns output_fd and closes it after this function returns.
// Return 0 for normal completion, -1 for an error.
int sensor_run(int output_fd, unsigned int interval_ms);

#endif
