//
//  client.h
//  MyFirstNetworkProgram
//
//  Created by samuel de santis.
//  Copyright (c) 2013 SDESANTIS. All rights reserved.
//

#ifndef MyFirstNetworkProgram_client_h
#define MyFirstNetworkProgram_client_h

#include <arpa/inet.h>      // inet_ntop()
#include <errno.h>          // errno
#include <netdb.h>          // addrinfo
#include <netinet/in.h>     // sockaddr_in
#include <stdbool.h>        // bool, true, false
#include <stdio.h>          // perror(), printf()
#include <stdlib.h>         // exit, EXIT_FAILURE
#include <string.h>         // memset(), strerror()
#include <sys/socket.h>     // AF_INET, SOCK_STREAM,
#include <unistd.h>         // close()

#include "config.h"

bool init_client()
{
    int status;
    struct addrinfo hints, *res, *it, tmp;
    int sockfd; // socket file descriptor
    char ipstr[INET6_ADDRSTRLEN]; // ip-string, use to store a printable ip address

    // first, load up address structs with getaddrinfo():
    
    // Initialises the struct with 0
    memset(&hints, 0, sizeof hints);
    
    // AF_UNSPEC make it IP version-agnostic.
    // Can use IPv4(AF_INET) or IPv6(AF_INET6), whichever.
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    
    // We use static host and port but can be dynamic
    const char *hostname = HOST;
    const char *servname = PORT;
    
    printf("--\n");
    printf("Addr info associated to host [%s] and service name [%s]\n",hostname,servname);
    
    status = getaddrinfo(hostname, servname, &hints, &res);
    if(status != GETADDRINFO_SUCCEED) {
        fprintf(stderr, "getaddrinfo failed, reason: %s\n",gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    
    // browses all addrinfo associated to the given host and service name
    for (it = res; it != NULL; it = it->ai_next) {
        void *addr;
        const char *ip_version;

        if (it->ai_family == AF_INET) { //IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)it->ai_addr;
            addr = &(ipv4->sin_addr);
            ip_version = "IPv4";
            tmp = *it;
        }
        else if (it->ai_family == AF_INET6) { //IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)it->ai_addr;
            addr = &(ipv6->sin6_addr);
            ip_version = "IPv6";
            tmp = *it;
        }
        else {
            continue;
        }

        // "ntop" stands for network to presentation (printable)
        inet_ntop(it->ai_family, addr, ipstr, sizeof(ipstr));
        // Print address
        printf("Address %s = %s\n", ip_version, ipstr);
    }
    
    printf("--\n");

    // browses all addrinfo associated to the given host and service name
    // and try to connect to the first one working.
    for (it = res; it != NULL; it = it->ai_next) {
        
        // make a socket:
        sockfd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        
        if (sockfd == SOCKET_ERROR) {
            fprintf(stderr, "socket failed, reason: %s\n",strerror(errno));
            continue;
        }

        fprintf(stdout, "socket creation succeed\n");
        
        // We don't call bind() !!
        // Basically, we don't care about our local port number; we only care where
        // we're going (the remote port). The kernel will choose a local port for
        // us, and the site we connect to will automatically get this information
        // from us. No worries.
        //
        //
        // bind it to the port we passed in to getaddrinfo():
        // bind(sockfd, res->ai_addr, res->ai_addrlen);
        //
        // int yes = 1;
        // if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&yes, sizeof(int)) == -1)
        // {
        //     perror("setsockopt");
        //     exit(EXIT_FAILURE);
        // }
        
        if(connect(sockfd, res->ai_addr, res->ai_addrlen) == SOCKET_ERROR)
        {
            perror("connect()");
            continue;
        }

        printf("Connected to remote server\n");
        
        char buffer[1024];
        int flags = 0; // no flags
        size_t length = sizeof(buffer);
        ssize_t bytes_recv = recv(sockfd, (void *)buffer, length, flags);
        if (bytes_recv > 0) {
            fprintf(stdout, "Got a new message:\n");
            fprintf(stdout, "\t%s [%ld chars]\n", buffer, bytes_recv);
        }
        else if (bytes_recv == 0) {
            printf("the remote side has closed the connection on you!");
        }
        else {
            perror("recv()");
        }
        
        close(sockfd);
        break;
    }
    
    freeaddrinfo(res);
    return true;
}

#endif
