#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void error(const char* msg) {

    // errno is a preprocessor macro used for error indication
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();

}

int main() {

    // create an socket and define fd with the file descriptor of the socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        error("socket()");
    }

    // bind the address with the local fd of the socket
    // create wildcard address 127.0.0.1:1234
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
    int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
    //printf("rv = %d\n", rv);
    if (rv) {
        error("connect()");
    }

    char msg[] = "hello";
    write(fd, msg, strlen(msg));

    char rbuf[64];
    ssize_t n = read(fd, rbuf, sizeof(rbuf) - 1);
    if (n < 0) {
        error("read()");
    }
    printf("server says: %s\n", rbuf);

    close(fd);

    return 0;

}