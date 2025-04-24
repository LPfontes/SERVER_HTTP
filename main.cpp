#include "TcpServerSocket.hpp"

int main()
{
    PSO::TcpServerSocket server(AF_INET, SOCK_STREAM, 0, 8080, INADDR_ANY);

    server.connect(server.get_sock(), server.get_address());
    server.start_listening(10); // até 10 conexões em espera
    server.accept_connection();

    return 0;
}
