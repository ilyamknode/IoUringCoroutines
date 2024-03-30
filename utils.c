//
// Created by user on 3/30/24.
//

#include "ring.h"
#include <stdio.h>
#include <stdlib.h>

int make_server(uint16_t port)
{
    struct sockaddr_in address = {0};

    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    address.sin_family = AF_INET;

    int fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (fd == -1) {
        perror("socket");
        exit(0);
    }

    if (bind(fd, (struct sockaddr*)&address, sizeof(address)) != 0) {
        perror("bind");
        exit(0);
    }

    if (listen(fd, 4096) != 0) {
        perror("listen");
        exit(0);
    }

    return fd;
}