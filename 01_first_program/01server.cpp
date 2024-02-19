#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>

void error(const char* msg) {

    int err = errno;
    printf("[%d] %s\n", err, msg);
    abort();

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
    int rv = bind(fd, (const sockaddr*)&addr, sizeof(addr));
    if (rv) {
        error("bind()");
    }


    close(fd);

    return 0;
}
