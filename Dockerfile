# Usa uma imagem base com g++
FROM ubuntu:24.04

# Instala dependências necessárias
RUN apt-get update && \
    apt-get install -y g++ make

# Cria um diretório de trabalho no container
WORKDIR /app

# Copia os arquivos do projeto para o container
COPY . .

# Compila o projeto
RUN g++ main.cpp TcpServerSocket.cpp Socket.cpp -o server

# Expõe a porta 8080
EXPOSE 8080

# Comando para iniciar o servidor
CMD ["./server"]
