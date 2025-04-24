# 🖼️ Servidor TCP para Upload e Redimensionamento de Imagens

Este projeto implementa um servidor TCP simples em C++ que permite:
- Enviar uma imagem via formulário HTML
- Redimensionar a imagem automaticamente (dobrando as dimensões originais)
- Fazer o download da imagem processada
- Suporte para JPEG e PNG
- Servido com sockets TCP puros (sem frameworks web!)

---

## Tecnologias

- C++
- HTML/CSS/JS
- Sockets TCP (POSIX)
- Bibliotecas [stb_image](https://github.com/nothings/stb)
- Docker

---
Estrutura

├── Dockerfile              # Define o ambiente do servidor C++

├── docker-compose.yaml     # Configura o serviço com Docker Compose

├── index.html              # Página de upload

├── main.cpp                # Ponto de entrada

├── TcpServerSocket.cpp/.hpp

├── RedimensionalImagem.cpp # Lógica de redimensionamento

├── stb_image_*             # Bibliotecas para imagem (sem dependências externas)

├── makefile

## Como usar

### 💻 Rodando localmente

#### 1. Compile o projeto:

```bash
make
```
2. Execute o servidor
```bash
./server
```
3. Acesse no navegador:
```bash
http://localhost:8080
```
ou 
1. Compile com Docker Compose:
```bash
docker-compose up --build
```
O container irá expor a aplicação na porta 8080.
