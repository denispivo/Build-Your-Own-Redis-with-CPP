# [Build Your Own Redis with CPP](https://build-your-own.org/redis/)

## Introduction to Sockets

```bash
socket()
```

- return an fd
-> fd is an integer that refers to something in the Linux kernel, like a TCP connection, a disk file, a listening port, or some other resources, etc.

```bash
bind()
```

- associates an address to a socket fd

```bash
listen()
```

- enables to accept connections to the given address

```bash
accept()
```

- takes a listening fd, when a client makes a connection to the listening address
- returns an fd that represents the connection socket