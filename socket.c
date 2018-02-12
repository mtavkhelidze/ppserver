/**
 * Copyright (c) 2018 Misha Tavkhelidze <misha.tavkhelidze@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "socket.h"

void _fill_peer_addr(peer_addr_t *peer, struct sockaddr *restrict addr)
{
    socklen_t plen = sizeof(peer->host);

    /**
     * As per man page, inet_ntop(3) fails only if
     * ss_family is wrong or the converted address string
     * would exceed the size given by last parameter.
     *
     * Any of those *should never happen* here.
     */
    if (addr->sa_family == AF_INET) {
        inet_ntop(addr->sa_family,
                  &((struct sockaddr_in *) addr)->sin_addr, peer->host, plen);
        peer->port = ((struct sockaddr_in *) addr)->sin_port;
    } else {
        inet_ntop(addr->sa_family, &((struct sockaddr_in6 *) addr)->sin6_addr,
                  peer->host, plen);
        peer->port = ((struct sockaddr_in6 *) addr)->sin6_port;
    }
}

socket_t *socket_init(const char *restrict host, const char *restrict port)
{
    int sd = -1;
    int flag = 1;
    struct addrinfo hints = { 0 };
    struct addrinfo *addr;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    do {
        int status;
        if ((status = getaddrinfo(host, port, &hints, &addr)) != 0) {
            fprintf(stderr, "Cannot get address info: %s\n",
                    gai_strerror(status));
            break;
        }

        if ((sd = socket(addr->ai_family, addr->ai_socktype,
                         addr->ai_protocol)) < 0) {
            perror("Cannot create socket");
            break;
        }

        int yes = 1;
        if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *) &yes,
                       sizeof(yes)) < 0) {
            perror("Cannot set socket options");
            break;
        };
    } while (--flag);

    if (flag != 0) {
        return NULL;
    }

    socket_t *server_socket;
    if ((server_socket = malloc(sizeof(socket_t))) == NULL) {
        perror("Cannot allocate memory");
        abort();
    }

    server_socket->sd = sd;
    server_socket->addr = addr;

    return server_socket;
}

void socket_cleanup(socket_t *pps)
{
    if (pps->addr != NULL)
        freeaddrinfo(pps->addr);

    close(pps->sd);

    free(pps);
}

int socket_connect(socket_t *restrict so)
{
    if (connect(so->sd, so->addr->ai_addr, so->addr->ai_addrlen) == 0) {
        return 0;
    }
    perror("Cannot connect");
    return -1;
}

int socket_listen(socket_t *restrict pps, unsigned int backlog)
{
    if (bind(pps->sd, pps->addr->ai_addr,
             pps->addr->ai_addrlen) != 0) {
        perror("Cannot bind");
        return -1;
    }

    if (listen(pps->sd, backlog) != 0) {
        perror("Cannot listen");
        return -1;
    }

    return 0;
}

int socket_accept(socket_t *server)
{
    int cfd;

    struct sockaddr_storage peer = { 0 };
    socklen_t peer_len = sizeof(struct sockaddr_storage);

    if ((cfd = accept(server->sd, (struct sockaddr *) &peer, &peer_len)) < 0) {
        return -1;
    }
    return cfd;
}

peer_addr_t *peer_addr(int pfd)
{
    peer_addr_t *peer;
    struct sockaddr_storage p = { 0 };
    socklen_t p_len = sizeof(struct sockaddr_storage);

    if (getpeername(pfd, (struct sockaddr *) &p, &p_len) != -1) {
        if ((peer = malloc(sizeof(peer_addr_t))) == NULL) {
            perror("Cannot allocate memory");
            abort();
        }
        _fill_peer_addr(peer, (struct sockaddr *) &p);
        return peer;
    };
    return NULL;
}

int set_connection_timeout(int pfd, int timeout)
{
    int ret = 0;
    struct timeval tv = { 0 };
    tv.tv_sec = timeout;
    if ((ret = setsockopt(pfd, SOL_SOCKET, SO_RCVTIMEO, (const void *) &tv,
                          sizeof(tv))) < 0) {
        perror("setsockopt(SO_RCVTIMEO) failed");
    }
    return ret;
}

int set_min_recv_len(int pfd, size_t len)
{
    int ret = 0;
    if ((ret = setsockopt(pfd, SOL_SOCKET, SO_RCVLOWAT, (const void *) &len,
                          sizeof(len))) < 0) {
        perror("setsockopt(SO_RCVLOWAT) failed");
    }
    return ret;
}
