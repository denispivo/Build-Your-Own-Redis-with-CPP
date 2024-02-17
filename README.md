# Build Your Own Redis with CPP

### learned with / got information from this [book](https://build-your-own.org/redis/)

#### What is a Socket?

A socket is a channel for two parties to communicate over a network.

#### TCP and UDP

TCP:
- is byte-stream-based
- a byte stream has no boundaries within it, so it makes understanding TCP harder
- Redis is TCP based

UDP:
- is packed-based
- a packet is a message of a given size

#### TCP/IP

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

#### Socker Primitives

Applications Refer to Sockets

- on Linux a handle is called *file desciptor* (fd), which is an integer that is unique to the process

```syscall
socket()
```

- a syscall that allocates and return a socket fd
- used to create a communication channel
- the handle must be closed when you´re done to free the associated resources on the OS side

#### Listening Socket & Connection Socket

