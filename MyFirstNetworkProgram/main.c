//
//  main.c
//  MyFirstNetworkProgram
//
//  A simple program that shows basic internet sockets programming using
//  TCP/IP and send()/recv().
//
//  Created by samuel de santis.
//  Copyright (c) 2013 SDESANTIS. All rights reserved.
//

#include "server.h"
#include "client.h"

#define SERVER_ROLE_ARG      "server"
#define CLIENT_ROLE_ARG      "client"

typedef enum {
    RoleUnknown,
    RoleClient,
    RoleServer,
} role_t;

//
// argv[1] should contain one of the role: server or client.
//
int main(int argc, const char * argv[]) {
    
    role_t role = RoleUnknown;
    
    if (argc>1) {
        if ( strcmp(argv[1], SERVER_ROLE_ARG)==0 ) {
            role = RoleServer;
        }
        else if ( strcmp(argv[1], CLIENT_ROLE_ARG)==0 ) {
            role = RoleClient;
        }
        else {
            printf("Unknown role. The role must be either 'server' or 'client'\n");
            return EXIT_FAILURE;
        }
    }
    else {
        printf("You have to specify a role (server or client)\n");
        return EXIT_FAILURE;
    }
    
    if (role == RoleClient) {
        printf("Start client\n");
        init_client();
    }
    else if (role == RoleServer) {
        printf("Start server\n");
        init_server();
    }
    
    return EXIT_SUCCESS;
}
