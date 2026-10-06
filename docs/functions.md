# Functions reference

Everything defined in `src/main.cpp`.

## `main(int argc, char* argv[])`

Entry point. Parses the CLI, forks an isolated child, and waits for it.

| Parameter | Meaning                                   |
| --------- | ----------------------------------------- |
| `argc`    | Argument count.                           |
| `argv`    | Arguments; `argv[0]` is the program name. |

What it does:

1. Build a `std::vector<char*>` command from argv, dropping the literal `run`
   token and rejecting unknown flags seen before the command.
2. Requires at least one command token; otherwise prints usage and returns 1.
3. Null-terminates the vector (execvp needs a null-terminated argv).
4. Declares a static stack and calls `clone()`.
5. On `clone()` failure, reports and returns 1.
6. Calls `waitpid()` and returns the child's exit code.

| Returns | Meaning                                                      |
| ------- | ------------------------------------------------------------ |
| 1       | Usage error, `clone()` error, or child exited with a signal. |
| other   | The child's exit code.                                       |

## `child(void* arg)`

The function `clone()` makes the child execute. Runs entirely inside the new
namespaces.

| Parameter | Meaning                                                              |
| --------- | -------------------------------------------------------------------- |
| `arg`     | Cast from the `&cmd` passed to `clone()`; the parsed command vector. |

What it does:

1. Casts `arg` back to `std::vector<char*>&`.
2. Calls `sethostname("minidock", 8)` to set the hostname (safe inside the UTS
   namespace).
3. Calls `mount("proc", "/proc", "proc", 0, nullptr)` to mount a private
   `/proc`.
4. Runs `execvp()` on the command. Only reaches the `perror`/return 1 path if
   exec fails.

| Returns | Meaning                                              |
| ------- | ---------------------------------------------------- |
| 0       | Only via execvp (never actually returns on success). |
| 1       | `sethostname`, `mount`, or `execvp` failed.          |

## Constants

### `FLAGS`

`CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | SIGCHLD`

The namespace and signaling bits passed to `clone()`.

### `STACK`

`1 << 20` (1 MiB). The size of the child's stack.
