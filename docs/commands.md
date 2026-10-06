# CLI reference

## `minidock run <command> [args...]`

Launches `<command>` in new PID, UTS, and mount namespaces, then waits for it.

| Argument              | Meaning                                                               |
| --------------------- | --------------------------------------------------------------------- |
| `run`                 | The subcommand. Everything after it is treated as the command to run. |
| `<command> [args...]` | The program to exec, e.g. `/bin/sh`, plus any args.                   |

Flags are only recognized _before_ the command. Once the first non-flag argument
is seen, every later argument (including ones starting with `-`) is passed
through to the command untouched.

### Behavior

1. Parse the command.
2. Call `clone()` with `CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | SIGCHLD`.
3. Inside the child: set hostname to `minidock`, mount a private `/proc`.
4. `execvp()` the command.
5. Parent blocks in `waitpid()`, then exits with the child's exit code.

### Errors

| Condition                   | Behavior              |
| --------------------------- | --------------------- |
| No command given            | Prints usage, exit 1. |
| Unknown flag before command | Prints error, exit 1. |
| `clone()` fails             | `perror`, exit 1.     |
| `execvp()` fails            | `perror`, exit 1.     |
| `waitpid()` fails           | `perror`, exit 1.     |

### Example

```sh
minidock run /bin/sh
```

Starts an isolated shell. `hostname` reads `minidock`; the shell is PID 1 in its
own PID namespace. Exiting the shell ends the process.
