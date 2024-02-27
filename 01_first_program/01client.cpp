#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/ip.h>

void error(const char *msg){

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

    // definde the address
    // needed is the <netinet/ip.h> library
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1

    close(fd);

    return 0;
}
