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
    FILE* output_file;
} ;

void SCHEDULER_decrementa_processos_restantes(SCHEDULER* e){
    e->processos_restantes -=1; 
}

SCHEDULER* SCHEDULER_cria(PoliticaEscalonamento politica, int quantum_ms,FILA* f , int quantidade_processos, FILE* output_file) {
    SCHEDULER* e = malloc(sizeof(SCHEDULER));
    e->fila = f;
    e->politica = politica;
    e->quantum_ms = quantum_ms;
    e->mutex = malloc(sizeof(pthread_mutex_t));
    e->cv = malloc(sizeof(pthread_cond_t));

    pthread_mutex_init(e->mutex, NULL);
    pthread_cond_init(e->cv, NULL);
    e->processos_restantes = quantidade_processos;
    e->todos_processos_chegaram = 0;
    e->processo_em_execucao = 0;
    e->output_file = output_file;
    return e;
}
PCB* SCHEDULER_seleciona_proximo_processo(SCHEDULER* e) {
    PCB* p = NULL;

    pthread_mutex_lock(e->mutex);

    if (e->politica == FCFS || e->politica == ROUND_ROBIN) {
        p = QUEUE_pop(e->fila);
    } else if (e->politica == PRIORIDADE) {
        p = QUEUE_pop(e->fila);
    }

    pthread_mutex_unlock(e->mutex);
    return p;
}

void* SCHEDULER_thread(void* arg) {
    SCHEDULER* e = (SCHEDULER*)arg;

    printf("[ESCALONADOR] Iniciado. Política: %s\n",
        e->politica == FCFS ? "FCFS" :
        e->politica == ROUND_ROBIN ? "ROUND ROBIN" :
        e->politica == PRIORIDADE ? "PRIORIDADE" : "DESCONHECIDA");

    while (1) {
        pthread_mutex_lock(e->mutex);
        while (QUEUE_vazia(e->fila) && !e->todos_processos_chegaram) {
            printf("[ESCALONADOR] Fila vazia. Esperando novos processos...\n");
            pthread_cond_wait(e->cv, e->mutex);
        }

        if (QUEUE_vazia(e->fila) && e->todos_processos_chegaram) {
            printf("[ESCALONADOR] Todos os processos chegaram e fila está vazia. Encerrando.\n");
            pthread_mutex_unlock(e->mutex);
            break;
        }
        pthread_mutex_unlock(e->mutex);

        PCB* proc = SCHEDULER_seleciona_proximo_processo(e);
        if (!proc) {
            printf("[ESCALONADOR] Nenhum processo retornado da fila. Continuando...\n");
            continue;
        }

        int pid = PCB_get_pid(proc);
        printf("[ESCALONADOR] Processo %d selecionado.\n", pid);

        pthread_mutex_t* mtx = PCB_get_mutex(proc);
        pthread_cond_t* cv = PCB_get_cond(proc);

        pthread_mutex_lock(mtx);
        if (PCB_get_estado(proc) == PRONTO) {
            PCB_set_estado(proc, EXECUTANDO);
            printf("[ESCALONADOR] Processo %d setado para EXECUTANDO. Acordando threads.\n", pid);
            pthread_cond_broadcast(cv);
        } else {
            printf("[ESCALONADOR] Processo %d não está pronto (estado: %d). Ignorado.\n", pid, PCB_get_estado(proc));
        }
        pthread_mutex_unlock(mtx);

        switch (e->politica) {
            case FCFS:
                printf("[ESCALONADOR] Aguardando finalização do processo %d (FCFS).\n", pid);
                fprintf(e->output_file,"[FCFS] Executando processo PID %d\n", pid);
                SCHEDULER_aguarda_finalizacao_processo(proc,e);
                printf("[ESCALONADOR] Processo %d finalizado.\n", pid);
                break;

            case ROUND_ROBIN:
                printf("[ESCALONADOR] (ROUND ROBIN) Executaria quantum para processo %d.\n", pid);
                // executar_quantum(proc, e->quantum_ms);
                break;

            case PRIORIDADE:
                printf("[ESCALONADOR] (PRIORIDADE) Executaria processo %d com prioridade.\n", pid);
                // executar_quantum(proc, e->quantum_ms);
                break;
        }
    }

    printf("[ESCALONADOR] Encerrado.\n");
    return NULL;
}


void SCHEDULER_aguarda_finalizacao_processo(PCB* pcb, SCHEDULER* e) {
    pthread_mutex_t* mutex = PCB_get_mutex(pcb);
    pthread_cond_t* cv = PCB_get_cond(pcb);

    pthread_mutex_lock(mutex);
    while (PCB_get_estado(pcb) != FINALIZADO) {
        pthread_cond_wait(cv, mutex);
    }
    pthread_mutex_unlock(mutex); // Desbloqueia o mutex do processo antes de mexer no escalonador

    // Protege o acesso à estrutura do escalonador
    pthread_mutex_lock(e->mutex);

    e->processos_restantes--;
    printf("%d \n", e->processos_restantes);
    if (e->processos_restantes == 0) {
        e->todos_processos_chegaram = 1;
        pthread_cond_broadcast(e->cv); // Acorda o escalonador, se ele estiver esperando
        printf("[ESCALONADOR] Todos os processos foram finalizados.\n");
    }

    pthread_mutex_unlock(e->mutex);
}

void  SCHEDULER_notifica_novo_processo(SCHEDULER* e){
    pthread_mutex_lock(e->mutex);
    pthread_cond_signal(e->cv); // acorda a thread do escalonador
    pthread_mutex_unlock(e->mutex);
}