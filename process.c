#include "process.h"

// Definição dos estados possíveis do processo
typedef enum { PRONTO, EXECUTANDO, FINALIZADO } EstadoProcesso;

struct PCB{

    int pid;                // ID único do processo
    int duracao_total;      // duração total do processo (em ms)
    int tempo_restante;     // quanto falta para o processo acabar (em ms)
    int prioridade;         // prioridade do processo (não usada ainda)
    int num_threads;        // quantas threads pertencem a este processo
    int tempo_chegada;      // tempo (ms) para o processo "chegar" no sistema
    EstadoProcesso estado;  // estado atual do processo

    pthread_mutex_t mutex;  // trava para evitar acesso concorrente
    pthread_cond_t cv;      // variável de condição para sincronizar as threads
    pthread_t* threads_ids; // array com os IDs das threads do processo
};

struct TCB{
    PCB* pcb;
    int indice_thread;
};

#define TEMPO_EXECUCAO_THREAD 500  // simula 500ms de execução

int tamanhoPcb() {
    return sizeof(PCB);
}

PCB* InicializaPCB(PCB* processo ,int pid, int duracao_total, int prioridade, int num_threads, int tempo_chegada) {
    
    if (processo == NULL) {
        perror("Erro ao alocar memória para o processo");
        exit(1);
    }

    processo->pid = pid;
    processo->duracao_total = duracao_total;
    processo->tempo_restante = duracao_total;
    processo->prioridade = prioridade;
    processo->num_threads = num_threads;
    processo->tempo_chegada = tempo_chegada;
    processo->estado = PRONTO;

    // Inicializa mutex e variável de condição
    pthread_mutex_init(&processo->mutex, NULL);
  

    if (pthread_cond_init(&processo->cv, NULL) != 0);


    // Aloca espaço para os IDs das threads
    processo->threads_ids = malloc(sizeof(pthread_t) * num_threads);

    return processo;
}

void destruirPCB(PCB* processo) {
    if (processo == NULL) return;

    pthread_mutex_destroy(&processo->mutex);
    pthread_cond_destroy(&processo->cv);
    free(processo->threads_ids);
    free(processo);
}

void* funcao_thread(void* arg) {
    TCB* tcb = (TCB*) arg;
    PCB* pcb = tcb->pcb;

    while (1) {
        pthread_mutex_lock(&pcb->mutex);
        while (pcb->estado != EXECUTANDO && pcb->estado != FINALIZADO) {
            // a thread entra em espera ate o estado dela mudar
            pthread_cond_wait(&pcb->cv, &pcb->mutex);
        }

        if (pcb->estado == FINALIZADO) {
            pthread_mutex_unlock(&pcb->mutex);
            break;
        }

        pthread_mutex_unlock(&pcb->mutex);
        usleep(TEMPO_EXECUCAO_THREAD * 1000); // simula 500 ms executando

        pthread_mutex_lock(&pcb->mutex);
        if (pcb->tempo_restante > 0) {
            pcb->tempo_restante -= TEMPO_EXECUCAO_THREAD;
            if (pcb->tempo_restante <= 0) {
                pcb->tempo_restante = 0;
                pcb->estado = FINALIZADO;
                pthread_cond_broadcast(&pcb->cv); // avisa todas threads para terminarem
            }
        }
        pthread_mutex_unlock(&pcb->mutex);
    }

    free(tcb);
    return NULL;
}

int getPcbTempoChegada(PCB* p){
    return p->tempo_chegada;
}

int PcbGetPrioridade(PCB*p){
    return p;
}