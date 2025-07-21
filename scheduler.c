#include <pthread.h>
#include "process.h" // Define PCB
#include "queue.h"


typedef enum { FCFS, ROUND_ROBIN, PRIORIDADE } PoliticaEscalonamento;

typedef struct {
    Fila* fila;
    PoliticaEscalonamento politica;
    int quantum_ms;
    pthread_mutex_t mutex;
    pthread_cond_t cv;
    int processos_restantes;
    int todos_processos_chegaram;
    int processo_em_execucao;
} Escalonador;

Escalonador* criar_escalonador(PoliticaEscalonamento politica, int quantum_ms) {
    Escalonador* esc = malloc(sizeof(Escalonador));
    esc->fila = criar_fila();
    esc->politica = politica;
    esc->quantum_ms = quantum_ms;
    pthread_mutex_init(&esc->mutex, NULL);
    pthread_cond_init(&esc->cv, NULL);
    esc->processos_restantes = 0;
    esc->todos_processos_chegaram = 0;
    esc->processo_em_execucao = 0;
    return esc;
}
PCB* selecionar_proximo_processo(Escalonador* esc) {
    PCB* p = NULL;

    pthread_mutex_lock(&esc->fila->mutex);

    if (esc->politica == FCFS || esc->politica == ROUND_ROBIN) {
        p = desenfileirar(esc->fila);
    } else if (esc->politica == PRIORIDADE) {
        p = remover_por_prioridade(esc->fila);
    }

    pthread_mutex_unlock(&esc->fila->mutex);
    return p;
}

void* thread_escalonador(void* arg) {
    Escalonador* esc = (Escalonador*)arg;

    while (1) {
        pthread_mutex_lock(&esc->mutex_fila);

        // Espera até ter processo ou todos os processos terem chegado
        while (fila_esta_vazia(esc->fila) && !esc->todos_processos_chegaram) {
            pthread_cond_wait(&esc->cv_fila, &esc->mutex_fila);
        }

        // Condição de término: nada mais a fazer
        if (fila_esta_vazia(esc->fila) && esc->todos_processos_chegaram) {
            pthread_mutex_unlock(&esc->mutex_fila);
            break;
        }

        // Seleciona o próximo processo da fila segundo a política
        PCB* proc = selecionar_proximo_processo(esc);
        pthread_mutex_unlock(&esc->mutex_fila);

        // Atualiza o estado e sinaliza as threads do processo
        pthread_mutex_lock(&proc->mutex);
        if (proc->estado == PRONTO) {
            proc->estado = EXECUTANDO;
            pthread_cond_broadcast(&proc->cv); // Acorda todas as threads do processo
        }
        pthread_mutex_unlock(&proc->mutex);

        // Executa conforme a política
        switch (esc->politica) {
            case FCFS:
                aguardar_finalizacao(proc);
                break;

            case ROUND_ROBIN:
                executar_quantum(proc, QUANTUM_MS);
                pthread_mutex_lock(&proc->mutex);
                if (proc->estado != FINALIZADO) {
                    proc->estado = PRONTO;
                    pthread_mutex_lock(&esc->mutex_fila);
                    reinserir_processo_fila(esc->fila, proc);
                    pthread_mutex_unlock(&esc->mutex_fila);
                }
                pthread_mutex_unlock(&proc->mutex);
                break;

            case PRIORIDADE:
                executar_quantum(proc, QUANTUM_MS);
                pthread_mutex_lock(&proc->mutex);
                if (proc->estado != FINALIZADO) {
                    proc->estado = PRONTO;
                    pthread_mutex_lock(&esc->mutex_fila);
                    reinserir_processo_fila(esc->fila, proc);
                    pthread_mutex_unlock(&esc->mutex_fila);
                }
                pthread_mutex_unlock(&proc->mutex);
                break;
        }
    }

    return NULL;
}