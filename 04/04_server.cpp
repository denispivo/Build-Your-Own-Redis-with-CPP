#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
//#include <netinet/ip.h>

const size_t k_max_msg = 4096;

static void error(const char* msg) {

    // errno is a preprocessor macro used for error indication
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();

}

static void msg(const char* msg) {

    fprintf(stderr, "%s\n", msg);

}

static int32_t one_request(int connfd) {

    // 4 byte header
    char rbuf[4 + k_max_msg + 1];

}

int main() {

    // create an socket and define fd with the file descriptor of the socket
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        error("socket() error");
    }

    int val = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // bind the address with the local fd of the socket
    // create wildcard address 0.0.0.0:1234
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0);
    int rv = bind(fd, (const sockaddr*)&addr, sizeof(addr));
    //printf("rv = %d\n", rv);
    if (rv) {
        error("bind() error");
    }

    //listen
    rv = listen(fd, SOMAXCONN);
    if (rv) {
        error("listen() error");
    }

    while (true) {
        //accept
        struct sockaddr_in client_addr;
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr*)&client_addr, &socklen);
        if (connfd < 0) {
            continue;
        }

        while (true) {

            int32_t err = one_request(connfd);
            if (err) {
                break;
            }


        }
        close(connfd);
    }

    close(fd);

    return 0;

}