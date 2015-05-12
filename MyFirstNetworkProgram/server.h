//
//  server.h
//  MyFirstNetworkProgram
//
//  Created by samuel de santis.
//  Copyright (c) 2013 SDESANTIS. All rights reserved.
//

#ifndef MyFirstNetworkProgram_server_h
#define MyFirstNetworkProgram_server_h

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

#define MESSAGE     "This is a message from the server!"
#define SIZE_BUF    1024 // maximum size of a message

bool init_server()
{
    int status;
    struct addrinfo hints, *res, *it;
    socket_t sockfd, csockfd; // socket file descriptor, client socket file descriptor
    struct sockaddr_storage their_addr;
    char ipstr[INET6_ADDRSTRLEN]; // ip string
    
    // first, load up address structs with getaddrinfo():
    
    // Initialises the struct with 0
    memset(&hints, 0, sizeof hints);
    
    // AF_UNSPEC make it IP version-agnostic.
    // Can use IPv4(AF_INET) or IPv6(AF_INET6), whichever.
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;     // fill in my IP for me
    
    const char *hostname = HOST;
    const char *servname = PORT;
    status = getaddrinfo(hostname, servname, &hints, &res);
    if(status != GETADDRINFO_SUCCEED) {
        fprintf(stderr, "getaddrinfo failed, reason: %s\n",gai_strerror(status));
        exit(EXIT_FAILURE);
    }
    
    printf("--\n");
    printf("Addr info associated to host [%s] and service name [%s]\n",hostname,servname);
    
    // browses all addrinfo associated to the given host and service name
    for (it = res; it != NULL; it = it->ai_next) {
        void *addr;
        const char *ip_version;
        
        if (it->ai_family == AF_INET) { //IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)it->ai_addr;
            addr = &(ipv4->sin_addr);
            ip_version = "IPv4";
        }
        else if (it->ai_family == AF_INET6) { //IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)it->ai_addr;
            addr = &(ipv6->sin6_addr);
            ip_version = "IPv6";
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
    // and bind to the first one working.
    for (it = res; it != NULL; it = it->ai_next) {
        
        // Create socket
        
        sockfd = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        
        if (sockfd == SOCKET_ERROR) {
            fprintf(stderr, "socket failed, reason: %s\n",strerror(errno));
            continue;
        }
        
        fprintf(stdout, "socket succeed\n");
        
        // Bind socket
        
        // Bind it to the port we passed in to getaddrinfo():
        bind(sockfd, it->ai_addr, it->ai_addrlen);
        
        // Prevent "address already in use" problem
        int yes = 1;
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *)&yes, sizeof(int)) == -1) {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        
        // Listen
        
        if(listen(sockfd, BACKLOG) == SOCKET_ERROR)
        {
            perror("listen()");
            break;
        }
        
        printf("Listen succeed\n");
        
        // now accept an incoming connection:
        
        socklen_t addr_size = sizeof(their_addr);
        csockfd = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
        
        printf("--\n");

        char hostname[SIZE_BUF];
        status = gethostname(hostname, SIZE_BUF);
        if(status != SOCKET_ERROR) {
            printf("Hostname:\t%s\n",hostname);
        }
        
        struct sockaddr_storage ss;
        socklen_t address_len = sizeof(ss);
        status = getpeername(csockfd, (struct sockaddr *)&ss, &address_len);
        if(status != SOCKET_ERROR) {
            
            if (ss.ss_family == AF_INET) {
                struct sockaddr_in *ipv4 = (struct sockaddr_in *)&ss;
                void *addr = &(ipv4->sin_addr);
                inet_ntop(ss.ss_family, addr, hostname, sizeof(hostname));
                printf("Peername:\t%s\n",hostname);
            }
            else if (ss.ss_family == AF_INET6) {
                struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)&ss;
                void *addr = &(ipv6->sin6_addr);
                inet_ntop(ss.ss_family, addr, hostname, sizeof(hostname));
                printf("Peername:\t%s\n",hostname);
                
                struct hostent *hp;
                struct in_addr ip;
                inet_pton(AF_INET6, hostname, (void*)&ip);
                if ((hp = gethostbyaddr((const void *)&ip,
                                        sizeof ip, AF_INET6)) == NULL) {
                    herror("gethostbyaddr()");
                }
                
                printf("name associated with %s is %s\n", ipstr, hp->h_name);
            }
        }
        
        // ready to communicate on socket descriptor csockfd!
        
        const char buffer[] = MESSAGE;
        size_t length = sizeof(buffer); // We want to transmit the null character
        int flags = MSG_NOFLAG;
        ssize_t bytes_sent = send(csockfd, buffer, length, flags);
        
        if (bytes_sent == SEND_ERROR) {
            perror("send()");
        }
        else {
            printf("Sent %ld characters\n",bytes_sent);
        }
        
        close(csockfd);
        close(sockfd);
        break;
    }
    
    freeaddrinfo(res);
    return true;
}


#endif
