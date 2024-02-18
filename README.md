# Build Your Own Redis with CPP

### learned with / got information from this [book](https://build-your-own.org/redis/)

1. [Socket Programming Concepts](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#socket-programming-concepts)

2. [TCP Server and Client](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#tcp-server-and-client)

## Socket Programming Concepts

### What is a Socket?

A socket is a channel for two parties to communicate over a network.

### TCP and UDP

TCP:
- is byte-stream-based
- a byte stream has no boundaries within it, so it makes understanding TCP harder
- Redis is TCP based

UDP:
- is packed-based
- a packet is a message of a given size

### TCP/IP

Layers of Protocols:
- network protocols have different layers, the higher layer is dependent on the lower one
- each layer has a different task

```network-layers
top
  /\    | App |     message or whatever
  ||    | TCP |     byte stream
  ||    | IP  |     packets
  ||    | ... |
bottom
```

- the IP layer is lower then the TCP layer
- one IP packet is a message with 3 components:
    - the sender´s address
    - the receiver´s address
    - the message data

### Socket Primitives

Applications Refer to Sockets

- on Linux a handle is called ***file desciptor*** (fd), which is an integer that is unique to the process

```syscall
socket()
```

- a syscall that allocates and return a socket fd
- used to create a communication channel
- the handle must be closed when you´re done to free the associated resources on the OS side

### Listening Socket & Connection Socket

A TCP server listens on a particular address (IP + port) and accepts client connections from that address. The listening address is also represented by a socket fd. And when you accept a new client connection, you get the socket fd of the TCP connection.

2 types of socket handles:
1. Listening sockets. Obtained by ***listening*** on an address.
2. Connection sockets. Obtained by ***accpting*** a client connection from a listening socket.

The relevent syscalls on Linux:

```syscall
bind()
```
- configure the listening address of a socket

```syscall
listening()
```
- make the socket a listening socket

```syscall
accept()
```
- return a client connection socket, when available

Pseudo code that explains the typical workflow of a server:
```psuedo-code
fd = socket()
bind(fd, address)
listen(fd)
while True:
    conn_fd = accept(fd)
    do_something_with(conn_fd)
    close(conn_fd)
```

### Read and Write

|Reading 	|Writing 	|Description                                    |
|:---------:|:---------:|:---------------------------------------------:|
|read 	    |write 	    |Read/write with a single continuous buffer.    |
|readv 	    |writev 	|Multiple Read/write in 1 syscall.              |
|recv 	    |send 	    |Has an extra flag.                             |
|recvfrom 	|sendto 	|Also get/set the remote address (packet-based).|
|recvmsg 	|sendmsg 	|readv/writev with more flags and controls.     |
|recvmmsg 	|sendmmsg 	|Multiple recvmsg/sendmmsg in 1 syscall.        |

- **read()** and **write()** are the most basic interfaces
- the rest are for additional controls and optimization

### Connect From a Client

```syscall
connect()
```
- is for initiating a TCP connection from the client side

Pseudo code:
```pseudo-code
fd = socket()
connect(fd, address)
do_something_with(fd)
close(fd)
```

The type of a socket (listening or connection) is determined after the **listen()** or **connect()** syscall.

### Summary

- Listening socket:
    - **bind()** and **lsiten()**
    - **accept()**
    - **close()**
- Using a socket:
    - **read()**
    - **write()**
    - **close()**
- Create a connection:
    - **connect()**

[back to the top](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#build-your-own-redis-with-cpp)

## TCP Server and Client

### Tips for Learning Socket Programming

```linux-command
man socket.2
```

- this Linux command shows the man page for the **socket()** syscall
- other examples:
    - **man read.2** returns the **read()** syscall (section 2 is for syscalls)
    - **man read** returns the **read** shell command (in section 1; not what you want)
    - **man socket.7** returns the socket interface overview, not the syscall
- use online resources such as [Bej´s Guide](https://beej.us/guide/bgnet/)

### Create a TCP Server

The server will read the data, given by the client, then write a response and close the connection.

#### Step 1: Obtain a Socket Handle

Use the **socket()** syscall like this:

```c++
int fd = socket(AF_INET, SOCK_STREAM, 0);
```

1. **AF_INET** is for IPv4. **AF_INET6** is for IPv6 or dual-stack sockets.
    - selects the IP level protocol
2. **SOCK_STREAM** is for TCP. **SOCK_DGRAM** is for UDP.
3. The 3rd argument is for the used protocol for that specific socket.
    - here it is 0, because it is useless for our purpose

How to create a **TCP** or **UDP** socket is documented in **man tcp.7** or **man udp.7**.

#### Step 2: Configuring the Socket

The syscall **setsockopt()** is used to configure the socket options after the socket has been created.

```c++
int val = 1;
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
```

- the 2nd and 3rd argument specifies which option to set
- the 4th argument is the option value
    - the option value is arbitrary bytes, so it´s length must be specified

Socket options are on several man pages.
TCP-specific optins are in **tcp.7** and many generic options are in **socket.7**.

Most options are optional, only the **SO_REUSEADDR** option needs to be used.
Without it **bind()** will fail when you restard your server.

#### Step 3: Bind to an Address

We’ll bind to the wildcard address **0.0.0.0:1234**.

```c++
// bind, this is the syntax that deals with IPv4 addresses
struct sockaddr_in addr = {};
addr.sin_family = AF_INET;
addr.sin_port = ntohs(1234);
addr.sin_addr.s_addr = ntohl(0);    // wildcard address 0.0.0.0
int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
if (rv) {
    die("bind()");
}
```

- **struct sockaddr_in** holds an IPv4 address and Port
    - initialize the structure as shown, with the **.sin_family**, **.sin_port** and **.sin_addr.s_addr**
- **ntohs()** and **ntohl()** functions convert numbers to the required big endian format

- use **struct sockaddr_in6** for IPv6
    - that´s why the **bind()** syscall needs the size of the structure

#### Step 4: Listening

The **listen()** syscall takes a **backlog** argument.

```c++
// listen
rv = listen(fd, SOMAXCONN);
if (rv) {
    die("listen()");
}
```

- after the **listen()** syscall the OS will automatically handle TCP handshakes and place established connections in queue
- retrieve them via **accept()**
- **backlog** argument is the size of the queue, here **SOMAXCONN**
    - **SOMAXCONN** is defined ad 128 on Linux

#### Step 5: Accept Connections

The server needs to enter a loop that accepts and processes each client connection.

```c++
while (true) {
    // accept
    struct sockaddr_in client_addr = {};
    socklen_t addrlen = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addrlen);
    if (connfd < 0) {
        continue;   // error
    }

    do_something(connfd);
    close(connfd);
}
```

- **accept()** syscall also returns the peer´s address
- **addrlen** argument is both the input size and the output size

#### Step 6: Read and Write

For the beginning the programm will only consist of 1 **read()** and 1 **write()**.

```c++
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

- **read()** and **write()** return the number of bytes, but we will ignore it here

### Create a TCP CLient

Almost the same as the server, but with the **connect()** syscall.

```c++
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

### Compile and Run

Compile the programm with this command lines:

```g++
g++ -Wall -Wextra -O2 -g 03_server.cpp -o server
g++ -Wall -Wextra -O2 -g 03_client.cpp -o client
```

**or**

Use the Makefile for easier compiling.

Run ./server in one window and then run ./client in another window:

```terminal
$ ./server
client says: hello
```

```terminal
$ ./client
server says: world
```

### More to Learn

[Go to 3.4](https://build-your-own.org/redis/03_hello_cs) and learn more about:
- **struct sockaddr**
- More Socket: Get Addresses
- More Socket: Configure My Local Address
- More Socket: Domain Name Resolution

[back to the top](https://github.com/denispivo/Build-Your-Own-Redis-with-CPP?tab=readme-ov-file#build-your-own-redis-with-cpp)