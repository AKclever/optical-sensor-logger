
#define _POSIX_C_SOURCE 200809L

// SPDX-License-Identifier: MIT
// Starter scaffold created with ChatGPT assistance.

#include "logger.h"

#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int logger_run(int input_fd, const char *output_path)
{
    // Open CSV file for writing.
    FILE *output = fopen(output_path, "w");

    if (output == NULL) {
        perror("fopen");
        return -1;
    }

    // Write CSV header.
    if (fprintf(output, "timestamp,intensity\n") < 0) {
        perror("CSV header");
        fclose(output);
        return -1;
    }

    // Duplicate the descriptor so the caller still owns input_fd.
    int stream_fd = dup(input_fd);

    if (stream_fd == -1) {
        perror("dup");
        fclose(output);
        return -1;
    }

    // Convert the duplicate into a buffered input stream.
    FILE *input = fdopen(stream_fd, "r");

    if (input == NULL) {
        perror("fdopen");
        close(stream_fd);
        fclose(output);
        return -1;
    }

    char line[128];
    int result = 0;

    // Continuously receive sensor readings.
    while (fgets(line, sizeof(line), input) != NULL) {

        size_t length = strlen(line);

        // Every record must end with a newline.
        if (length == 0 || line[length - 1] != '\n') {
            fprintf(stderr,
                    "Error: Incomplete or oversized sensor reading\n");
            result = -1;
            break;
        }

        // Parse timestamp and optical intensity.
        long long timestamp;
        double intensity;
        char extra;

        // Remove newline before validating the record.
        line[length - 1] = '\0';

        // Reject malformed records and extra characters.
        if (sscanf(line, "%lld,%lf%c",
                   &timestamp, &intensity, &extra) != 2) {
            fprintf(stderr,
                    "Error: Invalid sensor reading: %s\n", line);
            result = -1;
            break;
        }

        // Reject nonfinite values such as NaN and infinity.
        if (!isfinite(intensity)) {
            fprintf(stderr,
                    "Error: Nonfinite sensor intensity\n");
            result = -1;
            break;
        }

        // Write validated reading into CSV.
        if (fprintf(output, "%lld,%.2f\n",
                    timestamp, intensity) < 0) {
            perror("CSV write");
            result = -1;
            break;
        }
    }

    // Check whether reading failed.
    if (ferror(input)) {
        perror("pipe read");
        result = -1;
    }

    // Close the duplicated input descriptor.
    if (fclose(input) == EOF) {
        perror("close input stream");
        result = -1;
    }

    // Flush and close the CSV file.
    if (fclose(output) == EOF) {
        perror("close output file");
        result = -1;
    }

    return result;
}
