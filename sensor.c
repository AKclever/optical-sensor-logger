// SPDX-License-Identifier: MIT
// Starter scaffold created with ChatGPT assistance; no sensor logic implemented.
#include "sensor.h"

#include <errno.h>

int sensor_run(int output_fd, unsigned int interval_ms)
{
    (void)output_fd;
    (void)interval_ms;

    // TODO: Generate one simulated optical intensity and timestamp.
    // TODO: Format a bounded record ending in a newline.
    // TODO: Write the complete record, handling partial writes and EINTR.
    // TODO: Wait between samples without busy-looping.
    // TODO: Stop on a shutdown request; handle a closed reader (SIGPIPE/EPIPE).
    errno = ENOSYS; // Explicit placeholder: function not implemented.
    return -1;
}
