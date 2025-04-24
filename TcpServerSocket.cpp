#include "TcpServerSocket.hpp"

#include <vector>
#include <fstream>
#include <sstream>
#include "RedimensionaImagem.cpp"
using namespace PSO;

TcpServerSocket::TcpServerSocket(int domain, int service, int protocol, int port, u_long interface)
    : Socket(domain, service, protocol, port, interface) {}

int TcpServerSocket::connect(int sock, struct sockaddr_in address)
{
    // Usa bind para deixar o SO escolher a porta, se address.sin_port == 0
    int connection = bind(sock, (struct sockaddr*)&address, sizeof(address));

    // Captura a porta real que foi associada
    socklen_t addrlen = sizeof(address);
    getsockname(sock, (struct sockaddr *)&address, &addrlen); // read binding
    int local_port = ntohs(address.sin_port);  
    test_connection(sock, connection);
    printf("porta %d\n",local_port);
    return connection;
}

void TcpServerSocket::start_listening(int backlog)
{
    connection = listen(sock, backlog);
    test_connection(sock, connection);
    printf("Servidor ouvindo por conexões...\n");

}

void TcpServerSocket::accept_connection()
{
    while (true)
    {
        int addrlen = sizeof(address);
        int client_socket = accept(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        test_connection(sock, client_socket);
        printf("Conexão aceita com sucesso!\n");

        // Lê os dados da requisição em partes
        std::string request;
        char buffer[4096];
        int bytes_read;
        while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0) {
            request.append(buffer, bytes_read);
            if (request.find("\r\n\r\n") != std::string::npos) break;
        }

        // Verifica se é GET
        if (request.find("GET / ") == 0) {
            std::ifstream file("index.html");
            if (file.is_open()) {
                std::stringstream buffer;
                buffer << file.rdbuf();
                std::string content = buffer.str();
        
                std::string response =
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/html\r\n"
                    "Content-Length: " + std::to_string(content.length()) + "\r\n"
                    "\r\n" +
                    content;
        
                send(client_socket, response.c_str(), response.length(), 0);
            } else {
                const char* not_found =
                    "HTTP/1.1 404 Not Found\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Arquivo index.html não encontrado.";
                send(client_socket, not_found, strlen(not_found), 0);
            }
        }
        // Verifica se é POST
        else if (request.find("POST /") == 0) {
            // Continuar lendo o corpo (caso tenha vindo parcial)
            while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0) {
                request.append(buffer, bytes_read);
                if (request.find("--", request.size() - 8) != std::string::npos) break;
            }

            // Pega o boundary
            std::string boundary_key = "boundary=";
            size_t boundary_pos = request.find(boundary_key);
            if (boundary_pos == std::string::npos) {
                printf("Boundary não encontrado.\n");
                close(client_socket);
                continue;
            }

            std::string boundary = "--" + request.substr(
                boundary_pos + boundary_key.length(),
                request.find("\r\n", boundary_pos) - (boundary_pos + boundary_key.length())
            );

            // Procura início do conteúdo do arquivo
            size_t filename_pos = request.find("filename=");
            size_t file_start = request.find("\r\n\r\n", filename_pos);
            if (file_start == std::string::npos) {
                printf("Início do conteúdo do arquivo não encontrado.\n");
                close(client_socket);
                continue;
            }
            file_start += 4;

            // Procura fim do conteúdo do arquivo
            size_t file_end = request.find(boundary + "--", file_start);
            if (file_end == std::string::npos) {
                file_end = request.find(boundary, file_start);
            }
            if (file_end == std::string::npos) {
                printf("Fim do conteúdo do arquivo não encontrado.\n");
                close(client_socket);
                continue;
            }

            // Extrai o conteúdo da imagem
            std::string file_data = request.substr(file_start, file_end - file_start);
            if (file_data.size() >= 2 &&
                file_data[file_data.size() - 2] == '\r' &&
                file_data[file_data.size() - 1] == '\n') {
                file_data = file_data.substr(0, file_data.size() - 2);
            }

            std::string extension = "png"; // padrão
            size_t ext_start = request.find("filename=\"");
            if (ext_start != std::string::npos) {
                ext_start += 10;
                size_t ext_end = request.find("\"", ext_start);
                std::string filename = request.substr(ext_start, ext_end - ext_start);
                if (filename.find(".jpg") != std::string::npos) extension = "jpg";
                else if (filename.find(".jpeg") != std::string::npos) extension = "jpeg";
                else if (filename.find(".png") != std::string::npos) extension = "png";
            }
            std::string input_name = "upload_recebido." + extension;
            FILE* file = fopen(input_name.c_str(), "wb");
            if (file) {
                fwrite(file_data.c_str(), 1, file_data.size(), file);
                fclose(file);
                printf("Imagem salva como %s\n", input_name.c_str());
            } else {
                perror("Erro ao salvar o arquivo");
            }
            std::string output_name = "saida."+extension;
            // 3. Redimensiona a imagem
            redimensiona_imagem(input_name.c_str(), output_name.c_str());
            FILE* img = fopen(output_name.c_str(), "rb");
            if (!img) {
                const char* error =
                    "HTTP/1.1 500 Internal Server Error\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Erro ao processar imagem.";
                send(client_socket, error, strlen(error), 0);
                return;
            }
        
            fseek(img, 0, SEEK_END);
            long size = ftell(img);
            rewind(img);
            std::vector<char> buffer(size);
            fread(buffer.data(), 1, size, img);
            fclose(img);
        
            // 4. Retorna a imagem como resposta ao POST
            std::string header =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: image/"+extension+"\r\n"
                "Content-Length: " + std::to_string(size) + "\r\n\r\n";
        
            send(client_socket, header.c_str(), header.length(), 0);
            send(client_socket, buffer.data(), size, 0);
            remove(output_name.c_str());
            remove(input_name.c_str());
        }

        close(client_socket);
    }
}


