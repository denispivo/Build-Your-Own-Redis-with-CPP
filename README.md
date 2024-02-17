# Build Your Own Redis with CPP

### learned with / got information from this [book](https://build-your-own.org/redis/)

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

### Socker Primitives

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