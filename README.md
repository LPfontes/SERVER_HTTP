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

## Como usar

### 💻 Rodando localmente

#### 1. Compile o projeto:

```bash
make
