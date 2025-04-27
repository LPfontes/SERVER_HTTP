#include "TcpServerSocket.hpp"

#include <vector>
#include <fstream>
#include <sstream>
#include <iostream> 
#include "RedimensionaImagem.cpp" 

using namespace PSO;

// Construtor que apenas chama o construtor da classe base Socket
TcpServerSocket::TcpServerSocket(int domain, int service, int protocol, int port, u_long interface,int num_threads)
    : Socket(domain, service, protocol, port, interface), pool(num_threads) {}
// Implementa a função de conexão (na verdade faz o bind no servidor)
int TcpServerSocket::connect(int sock, struct sockaddr_in address)
{
    int connection = bind(sock, (struct sockaddr*)&address, sizeof(address));
    test_connection(sock, connection); // Testa se bind foi bem-sucedido
    std::cout << "porta " << ntohs(address.sin_port) << std::endl;
    return connection;
}

// Inicia a escuta de conexões
void TcpServerSocket::start_listening(int backlog)
{
    connection = listen(sock, backlog); // Começa a ouvir conexões
    test_connection(sock, connection);  // Verifica se o listen foi bem-sucedido
    std::cout << "Servidor ouvindo por conexões..." << std::endl;
}

void TcpServerSocket::accept_connection() {
    while (true) {
        int addrlen = sizeof(address);
        int client_socket = accept(sock, (struct sockaddr*)&address, (socklen_t*)&addrlen);
        test_connection(sock, client_socket);
        std::cout << "Conexão aceita com sucesso!" << std::endl;

        // Enfileira o tratamento da conexão no thread pool
        pool.enqueue([this, client_socket] {
            handle_client(client_socket);
        });
    }
}

// Novo método para lidar com o cliente (extraído do accept_connection original)
void TcpServerSocket::handle_client(int client_socket) {
    // Lê a requisição do cliente em blocos de 4096 bytes 
    std::string request;
    char buffer[4096];
    int bytes_read;
    while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0) {
        request.append(buffer, bytes_read);
        if (request.find("\r\n\r\n") != std::string::npos) break;
    }

    // Se for uma requisição GET
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
    // Se for uma requisição POST
    else if (request.find("POST /") == 0) {
            // Continua lendo o corpo do POST (caso não tenha sido todo lido)
            while ((bytes_read = read(client_socket, buffer, sizeof(buffer))) > 0) {
                request.append(buffer, bytes_read);
                if (request.find("--", request.size() - 8) != std::string::npos) break;
            }
        
            // Procura o boundary (separador de partes multipart)
            std::string boundary_key = "boundary=";
            size_t boundary_pos = request.find(boundary_key);
            if (boundary_pos == std::string::npos) {
                std::cout << "Boundary não encontrado." << std::endl;
                close(client_socket);
                
            }
        
            std::string boundary = "--" + request.substr(
                boundary_pos + boundary_key.length(),
                request.find("\r\n", boundary_pos) - (boundary_pos + boundary_key.length())
            );
        
            // Localiza onde começa o conteúdo do arquivo
            size_t filename_pos = request.find("filename=");
            size_t file_start = request.find("\r\n\r\n", filename_pos);
            if (file_start == std::string::npos) {
                std::cout << "Início do conteúdo do arquivo não encontrado." << std::endl;
                close(client_socket);
                
            }
            file_start += 4; // Pula os \r\n\r\n
        
            // Localiza onde termina o conteúdo do arquivo
            size_t file_end = request.find(boundary + "--", file_start);
            if (file_end == std::string::npos) {
                file_end = request.find(boundary, file_start);
            }
            if (file_end == std::string::npos) {
                std::cout << "Fim do conteúdo do arquivo não encontrado." << std::endl;
                close(client_socket);
                
            }
        
            // Extrai os bytes do arquivo recebido
            std::string file_data = request.substr(file_start, file_end - file_start);
            // Remove \r\n extra do final
            if (file_data.size() >= 2 &&
                file_data[file_data.size() - 2] == '\r' &&
                file_data[file_data.size() - 1] == '\n') {
                file_data = file_data.substr(0, file_data.size() - 2);
            }
        
            // Descobre a extensão do arquivo enviado
            std::string extension = "png"; // Default
            size_t ext_start = request.find("filename=\"");
            if (ext_start != std::string::npos) {
                ext_start += 10;
                size_t ext_end = request.find("\"", ext_start);
                std::string filename = request.substr(ext_start, ext_end - ext_start);
                if (filename.find(".jpg") != std::string::npos) extension = "jpg";
                else if (filename.find(".jpeg") != std::string::npos) extension = "jpeg";
                else if (filename.find(".png") != std::string::npos) extension = "png";
            }
        
            // Tipo de imagem para a função de redimensionamento
            bool type = extension.find("png") != std::string::npos ? true : false;
        
            // Salva a imagem recebida em disco
            std::string input_name = "upload_recebido." + extension;
            FILE* file = fopen(input_name.c_str(), "wb");
            if (file) {
                fwrite(file_data.c_str(), 1, file_data.size(), file);
                fclose(file);
                std::cout << "Imagem salva como " << input_name << std::endl;
            } else {
                perror("Erro ao salvar o arquivo");
            }
        
            // Redimensiona a imagem usando a função fornecida
            std::string output_name = "saida."+extension;
            redimensiona_imagem(input_name.c_str(), output_name.c_str(), type);
        
            // Lê a imagem redimensionada para enviar como resposta
            FILE* img = fopen(output_name.c_str(), "rb");
            if (!img) {
                const char* error =
                    "HTTP/1.1 500 Internal Server Error\r\n"
                    "Content-Type: text/plain\r\n\r\n"
                    "Erro ao processar imagem.";
                send(client_socket, error, strlen(error), 0);
                return;
            }
        
            // Determina o tamanho do arquivo de saída
            fseek(img, 0, SEEK_END);
            long size = ftell(img);
            rewind(img);
        
            // Carrega o conteúdo do arquivo para memória
            std::vector<char> buffer(size);
            fread(buffer.data(), 1, size, img);
            fclose(img);
        
            // Monta e envia a resposta HTTP com a imagem
            std::string header =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: image/" + extension + "\r\n"
                "Content-Length: " + std::to_string(size) + "\r\n\r\n";
        
            send(client_socket, header.c_str(), header.length(), 0);
            send(client_socket, buffer.data(), size, 0);
        
            // Remove arquivos temporários
            remove(output_name.c_str());
            remove(input_name.c_str());
        }

    // Fecha a conexão com o cliente
    close(client_socket);
}



