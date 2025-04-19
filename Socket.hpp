#ifndef SimpleSocket_hpp
#define SimpleSocket_hpp

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
namespace PSO
{
    class Socket
    {
    protected:
        struct sockaddr_in address;
        int sock;
        int connection;

    public:
        Socket(int domain, int service, int protocol, int port, u_long interface);
        virtual int connect(int sock, struct sockaddr_in address) = 0;
        void test_connection(int sock, int connection);

        // Getters
        struct sockaddr_in get_address();
        int get_sock();
        int get_connection();
    };
}

#endif
