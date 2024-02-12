# [Build Your Own Redis with CPP](https://build-your-own.org/redis/)

1. [Introduction to Sockets](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#introduction-to-sockets)

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

## Hello Server/Client

- obtain a socket fd like this: **int fd = socket(AF_INET, SOCK_STREAM, 0);**
- **AF_INET** is for IPv4, use **AF_INET6** for IPv6 or dual-stack socket
- **SOCK_STREAM** is for TCP

### server sided

```new-syscall
int val = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
```

```bash
setsockopt()
```

- is used to configure various aspects of a socket
- this particular call enables the **SO_REUSEADDR** option
    - without this option, the server won’t be able to bind to the same address if restarted
    - **SO_REUSEADDR** tries to use a sockt, but if it´s already used, it will use it anyway, if we don´t use **SO_REUSEADDR**, we will get an socket already in use error

```cpp
// bind, this is the syntax that deals with IPv4 addresses
struct sockaddr_in addr = {};
addr.sin_family = AF_INET;
addr.sin_port = ntohs(1234);
addr.sin_addr.s_addr = ntohl(0);    // wildcard address 0.0.0.0
int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
if (rv) {
    die("bind()");
}

// listen
rv = listen(fd, SOMAXCONN);
if (rv) {
    die("listen()");
}
```

- the next step is the **bind()** and **listen()**, we’ll bind on the wildcard address **0.0.0.0:1234**

```cpp
while (true) {
    // accept
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
    if (connfd < 0) {
        continue;   // error
    }

    do_something(connfd);
    close(connfd);
}
```

- loop for each connection and do something with them

```cpp
static void do_something(int connfd) {
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}
```

- the **do_something()** function is simply read and write
- note that the **read()** and **write()** call returns the number of read or written bytes

### client sided

```cpp
int fd = socket(AF_INET, SOCK_STREAM, 0);
if (fd < 0) {
    die("socket()");
}

struct sockaddr_in addr = {};
addr.sin_family = AF_INET;
addr.sin_port = ntohs(1234);
addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);  // 127.0.0.1
int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
if (rv) {
    die("connect");
}

char msg[] = "hello";
write(fd, msg, strlen(msg));

char rbuf[64] = {};
ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
if (n < 0) {
    die("read");
}
printf("server says: %s\n", rbuf);
close(fd);
```

```g++
g++ -Wall -Wextra -O2 -g 03_server.cpp -o server
g++ -Wall -Wextra -O2 -g 03_client.cpp -o client
```

- compile the programs with this command lines

```g++
$ ./server
client says: hello
```

```g++
$ ./client
server says: world
```

- run **./server** in a window and then run **./client** in another window