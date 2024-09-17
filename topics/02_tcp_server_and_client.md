## TCP Server and Client

[go back](/README.md)

1. [Tips for Learning Socket Programming](#tips-for-learning-socket-programming)
2. [Create a TCP Server](#create-a-tcp-server)
    1. [Step 1: Obtain a Socket Handle](#step-1-obtain-a-socket-handle)
    2. [Step 2: Configuring the Socket](#step-2-configuring-the-socket)
    3. [Step 3: Bind to an Address](#step-3-bind-to-an-address)
    4. [Step 4: Listening](#step-4-listening)
    5. [Step 5: Accept Connections](#step-5-accept-connections)
    6. [Step 6: Read and Write](#step-6-read-and-write)
3. [Create a TCP CLient](#create-a-tcp-client)
4. [Compile and Run](#compile-and-run)
5. [More to Learn](#more-to-learn)

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
    - **SOMAXCONN** is defined as 128 on Linux

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

[go back](/README.md)