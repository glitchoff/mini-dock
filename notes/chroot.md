# chroot

What `chroot()` does and how it fits MiniDock.

## What it is

A syscall that changes a process's root directory (`/`). After
`chroot("/some/dir")`, the process sees `/some/dir` as its new `/` and cannot
see anything outside it.

```cpp
chroot("/rootfs");   // now / is actually /rootfs
```

If `/rootfs` contains `bin/`, `dev/`, `etc/`, then inside:

- `/bin/sh` refers to `/rootfs/bin/sh`
- `ls /` shows only what's inside `/rootfs`
- `cd ..` from `/` stays at `/` (there is nothing above it)

## What it confines

A process's filesystem view to one directory tree. It cannot escape unless
running as root with `CAP_SYS_CHROOT` (there are `..` and `chroot`-escape
tricks, which is why real runtimes add namespaces on top).

## What it does NOT do

It does not touch processes, network, PIDs, or memory. It only changes the
filesystem view.

## How it fits MiniDock

Namespaces isolate processes/PIDs/mounts; chroot isolates the filesystem root.
This is how `--rootfs DIR` would work:

```cpp
chroot(DIR);   // confine the filesystem
chdir("/");    // move into the new root
```

That gives the container its own filesystem tree, like a tiny distro.

## Note on real containers

Linux prefers `pivot_root` + a mount namespace over raw `chroot` (it is safer).
`chroot` is the simpler thing to implement first, adding it here for now.

