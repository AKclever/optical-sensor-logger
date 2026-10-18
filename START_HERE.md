# Start here

This is a learning scaffold, not a completed assignment. ChatGPT supplied the
structure, build rules, interfaces, and TODO hints. You implement the logic.
The source/header files use the MIT SPDX license identifier.

## Build the scaffold

On Linux with a C compiler and Make installed:

```sh
make
./optlog
make clean
```

At this stage the program intentionally prints “Not implemented yet” to stderr
and exits with failure. The sensor and logger functions are unimplemented stubs
that return -1 with errno set to ENOSYS. Building successfully does not mean
the sensor logger works yet. Even --help remains your first exercise.

## What each file is for

| File | Your work |
| --- | --- |
| main.c | Arguments, pipe/fork, signals, descriptor ownership, waitpid, cleanup |
| sensor.c / sensor.h | Simulated readings and timed writes to the pipe |
| logger.c / logger.h | Receive records, validate them, and write CSV |
| Makefile | Build, clean, and optional clang-format commands |
| .gitignore | Keep binaries, object files, and generated readings out of Git |

The headers propose small interfaces; change them as you learn. The `(void)`
lines only suppress unused-parameter warnings while the functions are empty.
Remove them when you use those parameters.

## Suggested learning order

1. Implement --help and argument validation. Reject zero, negative, overflowing,
   and malformed sample intervals. Decide what to do with an existing CSV file.
2. Generate a single simulated reading before adding a timed loop. Pick and
   document timestamp units and the range of arbitrary intensity values.
3. Create a pipe and fork. Send one record, close the write end, receive it,
   and wait for the child. Draw which process owns each descriptor.
4. Save received records to CSV. Try multiple records and incomplete lines.
5. Add sampling intervals and clean signal-driven shutdown. Avoid doing file
   I/O in a signal handler; a sig_atomic_t flag is one possible starting point.
   Think about blocking reads/sleeps and how shutdown wakes them.
6. Handle failure paths: missing output directory, sensor exits, reader closes,
   interrupted system calls, write/flush failures. Reap the child on errors too.
7. Run make format (requires clang-format), update README with actual usage,
   tested Linux distribution, and limitations. Keep AI assistance comments.

Keep one intensity channel. No glucose estimation, threads, sockets, plotting,
or optical signal processing is needed. poll-based timeouts are optional later.

## Manual checks once implemented

- --help succeeds; invalid arguments fail with diagnostics on stderr.
- CSV has a header and complete readings at roughly the chosen interval.
- Ctrl+C leaves a readable CSV and no surviving or zombie child process.
- A bad output path fails without leaving the simulator running.
- EOF and a closed pipe do not cause a busy loop or a hung parent.

Before submission, replace proposal-only README text with what actually works.
