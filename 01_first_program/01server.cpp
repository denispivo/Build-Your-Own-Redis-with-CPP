#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>

void error(const char* msg) {

    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();

}

void msg(const char* msg) {

    fprintf(stderr, "%s\n", msg);

}

void do_something(int tempfd) {

    char rbuf[64] = {};
    // read the message and get the length of this message
    ssize_t n = read(tempfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        msg("error");
        return;
    }
    // print the message
    printf("client writes: %s\n", rbuf);

    // write a message to the client
    char wbuf[] = "world";
    write(tempfd, wbuf, strlen(wbuf));

}

int main() {

    // get the socket fd
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        error("socket()");
    }

    // configure the socket with the options you need
    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // definde the address
    // needed is the <netinet/ip.h> library
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); // 0.0.0.0
    // bind the address to the socket
    // rv is there to controll if any error happened
    int rv = bind(fd, (const sockaddr*)&addr, sizeof(addr));
    if (rv) {
        error("bind()");
    }

    //now listen for connections/messages on the socket fd
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        error("listen()");
    }

    // while loop to accept the incoming connections and process the connections
    while (true) {
        // temporarily store the client address
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr*)&client_addr, &addrlen);
        // if this if is satisfied the loop gets terminated and here the programm stops
        if (connfd < 0) {
            continue;
        }

        do_something(connfd);
        close(connfd);
    }

    close(fd);

    return 0;
}
