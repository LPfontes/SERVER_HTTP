#include <iostream>   
#include <vector>     
#include <thread>     
#include <mutex>      
#include <queue>      
#include <functional> 
#include <condition_variable> 

class ThreadPool {
    private:
    // Vetor para armazenar os objetos std::thread que representam as threads worker
    std::vector<std::thread> workers;
    // Fila para armazenar as tarefas (funções) a serem executadas
    std::queue<std::function<void()>> tasks;

    // Mutex para proteger o acesso à fila de tarefas e à variável 'stop'
    std::mutex queueMutex;
    // Variável de condição para permitir que as threads esperem por tarefas
    std::condition_variable condition;
    // Flag para sinalizar às threads que elas devem parar de executar
    bool stop = false;
public:
    // Construtor da classe ThreadPool
    // 'numThreads' especifica o número de threads que o pool irá gerenciar
    ThreadPool(size_t numThreads) {
        // Cria e inicia o número especificado de threads worker
        for(size_t i = 0; i < numThreads; ++i) {
            // 'emplace_back' constrói um novo std::thread diretamente no vector 'workers'
            // Cada thread executa uma função lambda
            workers.emplace_back([this] {
                // Loop infinito para que a thread continue trabalhando até que o pool seja destruído
                while(true) {
                    // 'task' irá armazenar a função a ser executada pela thread
                    std::function<void()> task;
                    // Bloco para proteger o acesso à fila de tarefas ('tasks') e à variável de condição ('condition')
                    {
                        // 'std::unique_lock' é um tipo de lock que permite o destravamento (unlock)
                        std::unique_lock<std::mutex> lock(queueMutex);

                        // 'condition.wait' faz com que a thread espere até que a condição seja notificada
                        // ou até que o predicado (a função lambda) retorne 'true'
                        condition.wait(lock, [this] {
                            // A thread continua a execução se a fila de tarefas não estiver vazia OU se o sinal de parada ('stop') for verdadeiro
                            return !tasks.empty() || stop;
                        });

                        // Se o sinal de parada foi acionado E a fila de tarefas está vazia, a thread termina sua execução
                        if(stop && tasks.empty()) {
                            return;
                        }

                        // Retira a primeira tarefa da fila
                        task = std::move(tasks.front());
                        tasks.pop();
                    } // O mutex 'queueMutex' é automaticamente destravado ao sair deste bloco

                    // Executa a tarefa que foi retirada da fila
                    task();
                }
            });
        }
    }

    // Método para adicionar uma nova tarefa à fila de execução
    // Recebe uma função 'f' (pode ser uma lambda, um ponteiro para função, ou um objeto função)
    template<class F>
    void enqueue(F&& f) {
        // Bloco para proteger o acesso à fila de tarefas
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            // Adiciona a função à fila de tarefas
            tasks.emplace(std::forward<F>(f));
        }
        // Notifica uma das threads em espera de que há uma nova tarefa na fila
        condition.notify_one();
    }

    // Destrutor da classe ThreadPool
    // Garante que todas as threads terminem sua execução antes que o objeto ThreadPool seja destruído
    ~ThreadPool() {
        // Sinaliza para as threads worker que elas devem parar
        {
            std::unique_lock<std::mutex> lock(queueMutex);
            stop = true;
        }
        // Notifica todas as threads em espera de que o sinal de parada foi acionado
        condition.notify_all();

        // Espera que todas as threads worker terminem sua execução
        for(std::thread &worker : workers) {
            worker.join();
        }
    }
};