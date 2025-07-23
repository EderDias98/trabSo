#include "process.h"

// Definição dos estados possíveis do p


struct PCB{

    int pid;                // ID único do p
    int duracao_total;      // duração total do p (em ms)
    int tempo_restante;     // quanto falta para o p acabar (em ms)
    int prioridade;         // prioridade do p (não usada ainda)
    int num_threads;        // quantas threads pertencem a este p
    int tempo_chegada;      // tempo (ms) para o p "chegar" no sistema
    EstadoProcesso estado;  // estado atual do p

    pthread_mutex_t* mutex;  // trava para evitar acesso concorrente
    pthread_cond_t* cv;      // variável de condição para sincronizar as threads
    pthread_t* threads_ids; // array com os IDs das threads do p
};

struct TCB{
    PCB* pcb;
    int indice_thread;
};

#define TEMPO_EXECUCAO_THREAD 500  // simula 500ms de execução

int PCB_get_tamanho() {
    return sizeof(PCB);
}

PCB* PCB_inicializa(PCB* p ,int pid, int duracao_total, int prioridade, int num_threads, int tempo_chegada) {
    
    if (p == NULL) {
        perror("Erro ao alocar memória para o p");
        exit(1);
    }

    p->pid = pid;
    p->duracao_total = duracao_total;
    p->tempo_restante = duracao_total;
    p->prioridade = prioridade;
    p->num_threads = num_threads;
    p->tempo_chegada = tempo_chegada;
    p->estado = PRONTO;

    // Inicializa mutex e variável de condição
    pthread_mutex_init(p->mutex, NULL);
  

    pthread_cond_init(p->cv, NULL);


    // Aloca espaço para os IDs das threads
    p->threads_ids = malloc(sizeof(pthread_t) * num_threads);

    return p;
}

void PCB_libera(PCB* p) {
    if (p == NULL) return;

    pthread_mutex_destroy(p->mutex);
    pthread_cond_destroy(p->cv);
    free(p->threads_ids);
    free(p);
}

void*  PCB_funcao_thread(void* arg) {
    TCB* tcb = (TCB*) arg;
    PCB* pcb = tcb->pcb;

    while (1) {
        pthread_mutex_lock(pcb->mutex);
        while (pcb->estado != EXECUTANDO && pcb->estado != FINALIZADO) {
            // a thread entra em espera ate o estado dela mudar
            pthread_cond_wait(pcb->cv, pcb->mutex);
        }

        if (pcb->estado == FINALIZADO) {
            pthread_mutex_unlock(pcb->mutex);
            break;
        }

        pthread_mutex_unlock(pcb->mutex);
        usleep(TEMPO_EXECUCAO_THREAD * 1000); // simula 500 ms executando

        pthread_mutex_lock(pcb->mutex);
        if (pcb->tempo_restante > 0) {
            pcb->tempo_restante -= TEMPO_EXECUCAO_THREAD;
            if (pcb->tempo_restante <= 0) {
                pcb->tempo_restante = 0;
                pcb->estado = FINALIZADO;
                pthread_cond_broadcast(pcb->cv); // avisa todas threads para terminarem
            }
        }
        pthread_mutex_unlock(pcb->mutex);
    }

    free(tcb);
    return NULL;
}

int PCB_get_tempo_chegada(PCB* p){
    return p->tempo_chegada;
}

int PCB_get_prioridade(PCB*p){
    return p->prioridade;
}
void PCB_create_threads(PCB* pcb) {



    for (int i = 0; i < pcb->num_threads; i++) {
        TCB* tcb = malloc(sizeof(TCB));

        tcb->pcb = pcb;
        tcb->indice_thread = i;

        pthread_create(&pcb->threads_ids[i], NULL, PCB_funcao_thread, (void*)tcb);
  
    }
}

void PCB_join_threads(PCB* p) {
    if (!p || !p->threads_ids) return;

    for (int i = 0; i < p->num_threads; i++) {
        pthread_join(p->threads_ids[i], NULL);
    }
}
EstadoProcesso PCB_get_estado(PCB* p){
    return p->estado;
}

void PCB_set_estado(PCB* p, EstadoProcesso esp){
    p->estado = esp;
}

pthread_mutex_t* PCB_get_mutex(PCB* p){
    return p->mutex;
}  // trava para evitar acesso concorrente
    
pthread_cond_t* PCB_get_cond(PCB* p){
    return p->cv;
}   