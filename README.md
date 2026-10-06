# MiniDock

A minimal Linux container runtime built directly from kernel primitives.

> **Status: work in progress.** This is a **learning project** built to
> understand container internals from first principles. It only implements
> process isolation so far; it is not a full container runtime yet.

## What it does

`minidock run` launches a process in new **PID**, **UTS**, and **mount**
namespaces, sets the hostname to `minidock`, mounts an isolated `/proc`, and
execs your command:

```sh
minidock run /bin/sh
```

Inside the container:

```sh
$ hostname
minidock

$ ps
PID  COMMAND
1    sh
```

Exit and the host is unaffected.

## Build & run (Docker)

```sh
docker build -t minidock .
# --privileged is required for the PID namespace and mount() call
docker run --privileged -it minidock minidock run /bin/sh
```

Run on a real Linux host directly:

```sh
cmake -S . -B build && cmake --build build
sudo ./build/minidock run /bin/sh
```

## Documentation

- [`docs/`](docs/) - reference for commands and functions
- [`notes/`](notes/) - study notes on the concepts and code
