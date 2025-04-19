#ifndef TcpServerSocket_hpp
#define TcpServerSocket_hpp

#include "Socket.hpp"

namespace PSO
{
    class TcpServerSocket : public Socket
    {
    public:
        TcpServerSocket(int domain, int service, int protocol, int port, u_long interface);
        int connect(int sock, struct sockaddr_in address) override;

        void start_listening(int backlog);
        void accept_connection();
    };
}

#endif
