#include "scheduler.h"



struct SCHEDULER{
    FILA* fila;
    PoliticaEscalonamento politica;
    int quantum_ms;
    pthread_mutex_t *mutex;
    pthread_cond_t *cv;
    int processos_restantes;
    int todos_processos_chegaram;
    int processo_em_execucao;
} ;

SCHEDULER* SCHEDULER_cria(PoliticaEscalonamento politica, int quantum_ms,FILA* f ) {
    SCHEDULER* e = malloc(sizeof(SCHEDULER));
    e->fila = f;
    e->politica = politica;
    e->quantum_ms = quantum_ms;
    e->mutex = malloc(sizeof(pthread_mutex_t));
    e->cv = malloc(sizeof(pthread_cond_t));

    pthread_mutex_init(&e->mutex, NULL);
    pthread_cond_init(&e->cv, NULL);
    e->processos_restantes = 0;
    e->todos_processos_chegaram = 0;
    e->processo_em_execucao = 0;
    return e;
}
PCB* SCHEDULER_seleciona_proximo_processo(SCHEDULER* e) {
    PCB* p = NULL;

    pthread_mutex_lock(&e->mutex);

    if (e->politica == FCFS || e->politica == ROUND_ROBIN) {
        p = QUEUE_pop(e->fila);
    } else if (e->politica == PRIORIDADE) {
        p = QUEUE_pop(e->fila);
    }

    pthread_mutex_unlock(&e->mutex);
    return p;
}


void* SCHEDULER_thread(void* arg) {
    SCHEDULER* e = (SCHEDULER*)arg;

    while (1) {
        // Espera até ter processo ou todos os processos terem chegado
        pthread_mutex_lock(&e->mutex);
        while (QUEUE_vazia(e->fila) && !e->todos_processos_chegaram) {
            pthread_cond_wait(&e->cv, &e->mutex);
        }

        // Condição de término: nada mais a fazer
        if (QUEUE_vazia(e->fila) && e->todos_processos_chegaram) {
            pthread_mutex_unlock(&e->mutex);
            break;
        }
        pthread_mutex_unlock(&e->mutex);

        // Seleciona o próximo processo da fila
        PCB* proc = SCHEDULER_seleciona_proximo_processo(e);
        if (!proc) continue;

        // Atualiza o estado do processo para EXECUTANDO
        pthread_mutex_t* mtx = PCB_get_mutex(proc);
        pthread_cond_t* cv = PCB_get_cond(proc);

        pthread_mutex_lock(mtx);
        if (PCB_get_estado(proc) == PRONTO) {
            PCB_set_estado(proc, EXECUTANDO);
            pthread_cond_broadcast(cv);  // Acorda todas as threads do processo
        }
        pthread_mutex_unlock(mtx);

        // Executa conforme a política
        switch (e->politica) {
            case FCFS:
                SCHEDULER_aguarda_finalizacao_processo(proc);
                break;

            case ROUND_ROBIN:
            case PRIORIDADE:
                // executar_quantum(proc, e->quantum_ms);
                // pthread_mutex_lock(mtx);
                // if (get_estado_pcb(proc) != FINALIZADO) {
                //     set_estado_pcb(proc, PRONTO);
                //     pthread_mutex_lock(&e->mutex);
                //     reinserir_processo_fila(e->fila, proc);
                //     pthread_mutex_unlock(&e->mutex);
                // }
                // pthread_mutex_unlock(mtx);
                // break;
        }
    }

    return NULL;
}

void SCHEDULER_aguarda_finalizacao_processo(PCB* pcb) {
    pthread_mutex_t* mutex = PCB_get_mutex(pcb);
    pthread_cond_t* cv = PCB_get_cond(pcb);

    pthread_mutex_lock(mutex);
    while (PCB_get_estado(pcb) != FINALIZADO) {
        pthread_cond_wait(cv, mutex);
    }
    pthread_mutex_unlock(mutex);
}

void  SCHEDULER_notifica_novo_processo(SCHEDULER* e){
    pthread_mutex_lock(&e->mutex);
    pthread_cond_signal(&e->cv); // acorda a thread do escalonador
    pthread_mutex_unlock(&e->mutex);
}