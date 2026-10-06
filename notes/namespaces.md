# Namespaces

What a namespace is and why `minidock run` uses three of them.

## What a namespace is

A re-scoped view of one kernel subsystem. A process in a new namespace sees a
different set of PIDs, hostnames, or mounts than the host does, even though it
is still just a normal host process underneath.

## The three used here

| Namespace | What it changes for the child                |
| --------- | -------------------------------------------- |
| PID       | The child becomes PID 1.                     |
| UTS       | `sethostname` changes only the child's view. |
| Mount     | Mounts are private to the child.             |

## Flags in `clone()`

```cpp
CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | SIGCHLD
```

| Flag           | Effect                                                           |
| -------------- | ---------------------------------------------------------------- |
| `CLONE_NEWPID` | New PID namespace.                                               |
| `CLONE_NEWUTS` | New UTS (hostname) namespace.                                    |
| `CLONE_NEWNS`  | New mount namespace.                                             |
| `SIGCHLD`      | Not a namespace; kernel signals the parent when the child exits. |

## Why each is needed

- **UTS** so `sethostname("minidock")` does not rename the actual host.
- **Mount** so the `/proc` mounted in the child is invisible to the host and
  `ps` inside shows only the container's processes.
- **PID** so the child appears as PID 1, not its real host PID.

## Key point

The isolation is perceptual. The host still sees the real PIDs and mounts; the
namespaces only change what the child observes.
