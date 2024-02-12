# [Build Your Own Redis with CPP](https://build-your-own.org/redis/)

## Introduction to Sockets

### server sided

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

```pseudo-code
fd = socket()
bind(fd, address)
listen(fd)
while True:
    conn_fd = accept(fd)
    do_something_with(conn_fd)
    close(conn_fd)
```

- pseudo-code that explains the typical workflow of a server

```bash
read()
```

- receives data from a TCP connection

```bash
write()
```

- sends data

```bash
close()
```

- destroys the resource referred by the fd and recycles the fd number

### client sided

```bash
connect()
```

- takes a socket fd and address and makes a TCP connection to that address

```pseudo-code
fd = socket()
connect(fd, address)
do_something_with(fd)
close(fd)
```

- pseudo-code for the client