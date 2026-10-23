// SPDX-License-Identifier: MIT
// Starter scaffold created with ChatGPT assistance.
#ifndef OPTICAL_LOGGER_H
#define OPTICAL_LOGGER_H

// Read newline-terminated timestamp,intensity records and save a CSV file.
// Timestamps are Unix seconds; intensity uses arbitrary simulation units.
// The caller owns input_fd. This function owns its output file and closes it.
// Return 0 for normal completion, -1 for an error.
int logger_run(int input_fd, const char *output_path);

#endif
