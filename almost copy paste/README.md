# Build Your Own Redis with CPP (links don´t work, except for book link)

### learned with / got information from this [book](https://build-your-own.org/redis/)

1. [Introduction to Sockets](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#introduction-to-sockets)
    - [server sided](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#server-sided)
    - [client sided](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#client-sided)

2. [Hello Server/Client](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#hello-serverclient)
    - [server sided](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#server-sided-1)
    - [client sided](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#client-sided-1)

3. [Protocol Parsing](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#protocol-parsing)
    - [Overview](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#overview)
    - [IO Helpers](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#io-helpers)
    - [The Parser](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#the-parser)
    - [The Client](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#the-client)
    - [Testing](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#testing)
    - [More on Protocol Design](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#more-on-protocol-design)
    - [Further Considerations](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#further-considerations)

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

#### [back to the top](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#build-your-own-redis-with-cpp)

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

- client programm

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

#### [finished task / client + server](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP/tree/master/03) -> folder 03

#### [back to the top](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#build-your-own-redis-with-cpp)

## Protocol Parsing

### Overview

Our server will be able to process multiple requests from a client, so we need to implement some sort of “protocol” to at least split requests apart from the TCP byte stream. The easiest way to split requests apart is to declare how long the request is at the beginning of the request.

```text
+-----+------+-----+------+--------
| len | msg1 | len | msg2 | more...
+-----+------+-----+------+--------
```

- use this scheme

The protocol consists of 2 parts: a 4-byte little-endian integer indicating the length of the request, and the variable-length request.

Starting from the code in the last chapter, the server’s loop is modified to handle multiple requests:

```cpp
while (true) {
    // accept
    struct sockaddr_in client_addr = {};
    socklen_t socklen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
    if (connfd < 0) {
        continue;   // error
    }

    // only serves one client connection at once
    while (true) {
        int32_t err = one_request(connfd);
        if (err) {
            break;
        }
    }
    close(connfd);
}
```

- **one_request** function parese only one request and responds, until somethin bad happen or the clients connction is lost
- at this time, the server can only handle one connection at a time

### IO Helpers

Adding two helper functions before listing the one_request function:

```cpp
static int32_t read_full(int fd, char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = read(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error, or unexpected EOF
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}

static int32_t write_all(int fd, const char *buf, size_t n) {
    while (n > 0) {
        ssize_t rv = write(fd, buf, n);
        if (rv <= 0) {
            return -1;  // error
        }
        assert((size_t)rv <= n);
        n -= (size_t)rv;
        buf += rv;
    }
    return 0;
}
```

Two things to note:

1. **read()** syscall just returns whatever data is available in the kernel, or blocks if there is none.
    **the read_full** function reads from the kernel until it geits exactly **n** bytes
2. **write()** syscall may return successfully with partial data written if the kernel buffer is full, we     must keep trying when the **write()** return fewer bytes than we need

### The Parser

The **one_request()** function did the heavy work:

```cpp
const size_t k_max_msg = 4096;

static int32_t one_request(int connfd) {
    // 4 bytes header
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(connfd, rbuf, 4);
    if (err) {
        if (errno == 0) {
            msg("EOF");
        } else {
            msg("read() error");
        }
        return err;
    }

    uint32_t len = 0;
    memcpy(&len, rbuf, 4);  // assume little endian
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }

    // request body
    err = read_full(connfd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }

    // do something
    rbuf[4 + len] = '\0';
    printf("client says: %s\n", &rbuf[4]);

    // reply using the same protocol
    const char reply[] = "world";
    char wbuf[4 + sizeof(reply)];
    len = (uint32_t)strlen(reply);
    memcpy(wbuf, &len, 4);
    memcpy(&wbuf[4], reply, len);
    return write_all(connfd, wbuf, 4 + len);
}
```

- For convenience, we put a limit on the maximum request size and use a buffer large enough to hold one request. 
- Endianness used to be a consideration when parsing protocols, but it is less relevant today so we are just **memcpy()**-ing integers.

### The Client

The client code for making requests and receiving responses:

```cpp
static int32_t query(int fd, const char *text) {
    uint32_t len = (uint32_t)strlen(text);
    if (len > k_max_msg) {
        return -1;
    }

    char wbuf[4 + k_max_msg];
    memcpy(wbuf, &len, 4);  // assume little endian
    memcpy(&wbuf[4], text, len);
    if (int32_t err = write_all(fd, wbuf, 4 + len)) {
        return err;
    }

    // 4 bytes header
    char rbuf[4 + k_max_msg + 1];
    errno = 0;
    int32_t err = read_full(fd, rbuf, 4);
    if (err) {
        if (errno == 0) {
            msg("EOF");
        } else {
            msg("read() error");
        }
        return err;
    }

    memcpy(&len, rbuf, 4);  // assume little endian
    if (len > k_max_msg) {
        msg("too long");
        return -1;
    }

    // reply body
    err = read_full(fd, &rbuf[4], len);
    if (err) {
        msg("read() error");
        return err;
    }

    // do something
    rbuf[4 + len] = '\0';
    printf("server says: %s\n", &rbuf[4]);
    return 0;
}
```

### Testing

```cpp
int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        die("socket()");
    }

    // code omitted ...

    // multiple requests
    int32_t err = query(fd, "hello1");
    if (err) {
        goto L_DONE;
    }
    err = query(fd, "hello2");
    if (err) {
        goto L_DONE;
    }
    err = query(fd, "hello3");
    if (err) {
        goto L_DONE;
    }

L_DONE:
    close(fd);
    return 0;
}
```

Running the server and the client:

```running_code
$ ./server
client says: hello1
client says: hello2
client says: hello3
EOF

$ ./client
server says: world
server says: world
server says: world
```

### More on Protocol Design

There are many decisions to be made when designing protocols. You can learn the tradeoffs by looking at existing protocols.

#### Text vs. Binary

The first decision in protocol design is text vs binary. Text protocols have the advantage of being human-readable, making debugging easier. A notable example is the HTTP protocol (not the newer one).

A disadvantage of text protocols is their complexity, even the simplest text protocol requires more work and is more error-prone to parse. You can try to compare the real Redis protocol with the protocol of this book.

Why are text protocols harder to parse? Because they consist of variable-length strings, the text parsing code involves a lot of length calculations, bound checks, and decisions. Let’s say you want to parse an integer in decimal text “1234”, for every byte, you have to check for the end of the buffer and whether the integer has ended. This is in contrast to the simplicity of a fixed-width binary integer.

The above example leads to a protocol design tip: avoid unnecessary variable-length components. The fewer of them, the less parsing, and the fewer of security bugs. Let’s say you want to include a string in your protocol, consider whether a fixed-length string is acceptable, or whether the string is needed at all.

#### Streaming Data

Our protocol includes the length of the message at the beginning. However, real-world protocols often use less obvious ways to indicate the end of the message. Some applications support “streaming” data continuously without knowing the full length of the output. A good example is the “Chunked Transfer Encoding” from the HTTP protocol.

Chunked encoding wraps incoming data into a message format that starts with the length of the message. The client receives a stream of messages, until a special message indicates the end of the stream.

There is also an alternative but bad way to do this: Use a special character (delimiter) to indicate the end of the stream. The problem is that the payload data encoding can not contain the delimiter, you need some “escape” mechanism, which complicates things.

### Further Considerations

The protocol parsing code requires at least 2 **read()** syscalls per request. The number of syscalls can be reduced by using “buffered IO”. That is, read as much as you can into a buffer at once, then try to parse multiple requests from that buffer. Readers are encouraged to try this as an exercise as it may be helpful in understanding later chapters.

There are some common beginner mistakes when designing or implementing protocols:

```text
Mistake 1: Not handling the return value of **read()** and **write()**.
```

These two functions can return fewer bytes than you expected, see the notes on the **read_full()** helper. This mistake is also common with an event loop.

```text
Mistake 2: No way to indicate the end of the message.
```

People often believe that the **read()** and **write()** syscalls are “messages” instead of byte streams, resulting in an unparsable protocol. Early versions of HTTP also allow this flaw: an HTTP connection without the **Content-Length** header or chunked encoding cannot be used for multiple requests.

```text
Mistake 3: Unnecessary complexities.
```

See the section on the protocol design.

#### [back to the top](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#build-your-own-redis-with-cpp)

