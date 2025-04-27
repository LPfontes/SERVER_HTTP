# Nome do executável final
TARGET = server

# Compilador e flags
CXX = g++
CXXFLAGS = -Wall -std=c++11

# Arquivos fonte e objetos
SRCS = main.cpp TcpServerSocket.cpp Socket.cpp ThreadPool.cpp
OBJS = $(SRCS:.cpp=.o)

# Regra padrão
all: $(TARGET)

# Como construir o executável
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Como compilar os arquivos .cpp em .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpa os arquivos de build
clean:
	rm -f *.o $(TARGET)
