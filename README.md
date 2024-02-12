# [Build Your Own Redis with CPP](https://build-your-own.org/redis/)

## Introduction to Sockets

```bash
socket()
```

- return an fd
-> an fd is an integer that refers to something in the Linus kernel, like a TCP connection, a disk file, a listening port, or some other resources, etc.

```bash
bind()
```

- associates an address to a socket fd

```bash
listen()
```