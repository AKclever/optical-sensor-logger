# Optical Sensor Logger

A small Linux C system-programming project for ITS8020: record a simulated optical sensor stream using Unix processes and pipes.

## Status

Project proposal and initial repository. Implementation has not started.

## Planned scope

- A child process generates one simulated light-intensity value at a configurable interval.
- A pipe carries readings to the parent process.
- The parent records readings in a CSV file.
- Ctrl+C stops acquisition cleanly, closes resources, and collects the child exit status.
- Command-line options provide help, sample interval, and output filename.
- Errors are reported to stderr with appropriate exit codes.

The readings are synthetic optical intensity values, not glucose estimates or medical measurements.

## Planned structure

Multiple C source files and headers, with a Makefile, .gitignore, and license information before submission. Linux distribution and build/run instructions will be documented after implementation and testing.

## Optional extension

Input timeout detection with poll, after the basic logger works.

## Assistance

The project idea and this initial README were developed with ChatGPT assistance.
