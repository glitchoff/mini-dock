# Headers used in src/main.cpp

What each include pulls in and why MiniDock uses it.

| Header | What it provides | Used for |
| ------ | ---------------- | -------- |
| `<unistd.h>` | POSIX OS API | `sethostname()`, `execvp()`, `getpid()` |
| `<sched.h>` | Scheduling / process creation | `clone()`, `CLONE_NEWPID`, `CLONE_NEWUTS`, `CLONE_NEWNS` |
| `<sys/wait.h>` | Waiting on children | `waitpid()`, `WIFEXITED()`, `WEXITSTATUS()` |
| `<sys/mount.h>` | Mount syscall | `mount()` to set up `/proc` |
| `<cstdio>` | C stdio | `printf()`, `fprintf()`, `perror()` |
| `<string>` | `std::string` | Store command tokens and args |
| `<vector>` | Dynamic array | Hold the parsed command and its args |

## Grouped by purpose

**Process creation and isolation** come from `<unistd.h>`, `<sched.h>`, and
`<sys/wait.h>`: clone the process with namespaces, run the command, wait for it.

**Filesystem** comes from `<sys/mount.h>`: mount a private `/proc` inside the
child's new mount namespace.

**I/O and data** come from `<cstdio>`, `<string>`, and `<vector>`: print
diagnostics, store the command tokens, and hold them for exec.

## Why fprintf instead of printf?

`printf()` writes to **stdout**, the normal output stream. `fprintf(stderr, ...)`
writes to **stderr**, the error stream.

- Normal program output (like the `MiniDock: container runtime` banner) uses
  `printf`.
- Errors and usage messages use `fprintf(stderr, ...)` so they are kept
  separate from real output. That matters when stdout is redirected: a script
  piping output to a file still sees errors on the terminal (or wherever
  stderr points).

This is the standard Unix convention: send data to stdout, send diagnostics to
stderr. `perror()` does the same thing by always writing to stderr.