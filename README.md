# Optical Sensor Logger

I have been interested in the hardware inside health gadgets, especially how a sensor reading becomes data that software can use. I chose this project as a small first step towards understanding that process: producing readings, passing them between processes, and saving them so they can be inspected later. Eventually I would like to explore reading from actual hardware, but for this assignment I started with a simulated optical sensor.

This is my C system-programming project for ITS8020. It runs on Linux and uses a child process to generate readings, a pipe to send them to the parent, and a CSV file to store them. The values are simulated light-intensity readings in arbitrary units. They do not represent measurements from a health device or estimates of glucose or any other medical value.

## Build and run

You need Make and a C compiler with C11 and POSIX support. The current version was built and checked in a Linux x86_64 environment running Ubuntu 24.04.3 LTS with GCC 13.3.0. These checks were run with ChatGPT assistance; other distributions have not been verified.

From the project directory:

```sh
make
./optlog --help
./optlog --interval 250 --output readings.csv
```

The last command takes a reading roughly every 250 milliseconds. Press Ctrl+C to stop. The parent lets the child stop, finishes reading the pipe, closes the CSV file, and collects the child's exit status. Sending SIGINT or SIGTERM to the parent also requests a clean shutdown.

With no options, `./optlog` uses a 1000 ms interval and writes to `readings.csv` in the current directory.

| Option | Meaning |
| --- | --- |
| `--help` | Print usage and exit successfully |
| `--interval MS` | Positive whole number of milliseconds, up to `INT_MAX` (2147483647 on the tested system) |
| `--output PATH` | CSV file to create or overwrite; its directory must already exist |

**An existing output file is overwritten.** Use a different filename if you want to keep an earlier recording. Diagnostics go to stderr, and errors return a nonzero exit status. Normal logging is quiet; the readings go into the file.

## What gets recorded

The CSV looks like this:

```csv
timestamp,intensity
1792368000,42.50
1792368001,42.60
1792368002,42.70
```

The timestamp is Unix time in whole seconds. The simulated intensity cycles from 42.50 to 43.40 in steps of 0.10, then starts again. This predictable pattern makes it easy to check that records are arriving in order. With intervals shorter than one second, several readings can have the same timestamp.

Each row is flushed to the file as it arrives, so you can inspect a recording while it runs:

```sh
tail -f readings.csv
```

Sampling uses a relative sleep after each reading, so the interval is approximate and can drift. Flushing makes data visible to other programs but does not guarantee it has reached the physical disk. There is no real sensor input, calibration, or analysis yet.

## Files

| File | Responsibility |
| --- | --- |
| `main.c` | Options, pipe and process setup, shutdown, and child cleanup |
| `sensor.c` / `sensor.h` | Simulated readings and timed writes |
| `logger.c` / `logger.h` | Reading and checking records, writing the CSV |
| `Makefile` | Build, clean, and formatting commands |
| `START_HERE.md` | A short guide for following and checking the code |

Run `make clean` to remove the executable and object files. It keeps your CSV recordings. `make format` formats the C files and headers and requires `clang-format` to be installed.

## Checks

The current version was checked for help output, invalid and overflowing intervals, missing options, CSV content, live row visibility, and output-path errors. Shutdown was checked using terminal-style Ctrl+C and SIGINT/SIGTERM sent just to the parent. A write to `/dev/full` was used to check that output failures return an error.

## ChatGPT assistance

I used ChatGPT for the project idea and initial scaffold, including the build rules and interfaces. ChatGPT also reviewed the code, helped implement the signal-forwarding and startup-race fixes, added CSV flush checks, ran verification checks, and helped rewrite this documentation. Comments near those changes identify the assistance. The commit history keeps the earlier scaffold and later changes visible.

## License

The C source files and headers use the MIT license, identified by their `SPDX-License-Identifier: MIT` comments.
