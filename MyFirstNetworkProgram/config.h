//
//  config.h
//  MyFirstNetworkProgram
//
//  Created by samuel de santis.
//  Copyright (c) 2013 SDESANTIS. All rights reserved.
//

#ifndef MyFirstNetworkProgram_config_h
#define MyFirstNetworkProgram_config_h

#define INVALID_SOCKET      (-1)
#define SOCKET_ERROR        (-1)
#define SEND_ERROR          (-1)
#define HOST                "localhost"
#define PORT                "1988"		// 32 bit port
#define GETADDRINFO_SUCCEED (0)
#define MSG_NOFLAG          (0x00)
#define BACKLOG             (10) // maximum length for the queue of pending connections

// socket type
typedef int         socket_t;

#endif
