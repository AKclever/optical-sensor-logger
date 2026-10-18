// SPDX-License-Identifier: MIT
// Starter scaffold created with ChatGPT assistance; no logging logic implemented.
#include "logger.h"

#include <errno.h>

int logger_run(int input_fd, const char *output_path)
{
    (void)input_fd;
    (void)output_path;

    // TODO: Open the output file and write a CSV header; check failures.
    // TODO: Receive records. One read is NOT necessarily one complete record.
    // Hint: fdopen + fgets can simplify line buffering; fdopen transfers
    // descriptor ownership, so adjust the contract/caller if you choose it.
    // TODO: Validate records and write them to the output file.
    // TODO: Handle EOF, interrupted input, shutdown, and output errors.
    // TODO: Check close/flush errors too; keep diagnostics on stderr.
    errno = ENOSYS;
    return -1;
}
