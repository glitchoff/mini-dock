# Process creation: fork, clone, exec, waitpid

The process primitives `minidock run` uses.

## fork() vs clone()

`fork()` creates a new process by copying the caller: same code, same stack,
same memory. The only difference is the return value:

- Parent gets the child's PID.
- Child gets `0`.

`clone()` does the same, but lets you pick what is shared or copied via flags.
The child can run a different function on a different stack.

|               | fork()               | clone()                            |
| ------------- | -------------------- | ---------------------------------- |
| Child code    | Same as parent       | A new function                     |
| Child stack   | Copied from parent   | You must provide one               |
| What's shared | Everything is copied | Chosen by flags (namespaces, etc.) |

## Why clone() needs a stack

Because the child runs a new function, it cannot reuse the parent's stack (that
memory belongs to the parent's call frame). You pass a region of memory for the
child to run on:

```cpp
static char stack[STACK];                 // static -> lives in BSS, not heap
pid_t pid = clone(child, stack + STACK, FLAGS, &cmd);
```

`stack + STACK` is the top of the stack because stacks grow downward.

## clone() return semantics

- Returns child PID to the parent.
- The child starts executing `child()` as if it had returned from `clone()`.

## Why execvp()?

`clone()` still just duplicates a process. The child keeps running our code.
`execvp()` replaces the current process image with another program:

```cpp
execvp(cmd[0], cmd.data());   // become /bin/sh; only returns on failure
```

So the pattern is to create a fresh, isolated process (clone), set it up, then
hand it off to a real program (exec). This is the classic fork/exec model.

## Why waitpid()?

The parent must not exit while the child runs, or the child gets orphaned.
`waitpid()` blocks the parent until the child exits, reaps it (prevents a
zombie), and lets us forward the exit code:

```cpp
int status;
waitpid(pid, &status, 0);
return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
```

## The full picture

```
clone()  ->  fresh, isolated child process (still running our code)
  ├── sethostname()   setup
  ├── mount()         setup
  └── execvp()        child becomes the real command
parent: waitpid() blocks until that child exits
```

## Quiz yourself

1. Why must the stack be `static`?
2. What happens if we skip `execvp()`?
3. What happens if we skip `waitpid()`?
