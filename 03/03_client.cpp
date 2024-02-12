#include <stdio.h>
#include <stdlib.h>
//#include <stdint.h>
//#include <string>
//#include <unistd.h>
#include <errno.h>
//#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void error(const char *msg) {

    // errno is a preprocessor macro used for error indication
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();

}

int main() {

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
        error("socket()");
    }

    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = 1234;
    addr.sin_addr.s_addr = INADDR_LOOPBACK; // 127.0.0.1

    return 0;

}