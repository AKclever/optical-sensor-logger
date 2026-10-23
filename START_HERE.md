# Following the code

Start with the build and usage examples in [README.md](README.md). This file is a guide to the implementation rather than a list of unfinished exercises.

1. In `main.c`, follow the options first. Invalid arguments are rejected before starting the child or opening the output file.
2. Follow the two pipe descriptors across `fork()`. The child keeps the writing end and the parent keeps the reading end. Closing the unused ends is important: the logger needs EOF when the sensor stops.
3. In `sensor.c`, look at the stop flag, reading format, write loop, and interrupted sleep handling.
4. In `logger.c`, follow one line from `fgets()` through validation and into the CSV file. The duplicated input descriptor lets the caller keep ownership of its original descriptor.
5. Return to `main.c` for shutdown. The parent forwards SIGINT/SIGTERM to the child and continues draining the pipe before calling `waitpid()`. Stop signals are blocked during setup so neither process handles them before it is ready.

ChatGPT helped with the initial scaffold and the later shutdown, flush-check, documentation, and verification changes. The relevant source comments and README describe that assistance.

## Try it

```sh
make
./optlog --help
./optlog --interval 100 --output test.csv
# Press Ctrl+C, then inspect the file:
head test.csv
./optlog --interval 0
./optlog --output /a-directory-that-does-not-exist/readings.csv
./optlog --output /dev/full
```

Help should succeed. Invalid arguments and output failures should print diagnostics to stderr and return a nonzero status. Check the previous command's status with `echo $?`.

To check a signal sent only to the parent:

```sh
./optlog --interval 100 --output signal-test.csv &
logger_pid=$!
sleep 1
kill -TERM "$logger_pid"
wait "$logger_pid"
echo $?
```

This should finish with status 0 and a readable CSV. Repeat with `kill -INT` to check SIGINT. Neither test should leave the sensor child running.
