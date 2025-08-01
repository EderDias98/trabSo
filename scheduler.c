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
    int escalonador_esperando;
    int escalonador_vazio;

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
    e->escalonador_esperando = 0;
    e->escalonador_vazio =0;


    return e;
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
            
            e->escalonador_esperando =1;
            
            pthread_cond_signal(e->cv);
            printf("[ESCALONADOR] Fila vazia. Esperando novos processos...\n");
            pthread_cond_wait(e->cv, e->mutex);
            
         
        }
        e->escalonador_esperando =0;
      
        


        if (QUEUE_vazia(e->fila) && e->todos_processos_chegaram) {
            printf("[ESCALONADOR] Todos os processos chegaram e fila está vazia. Encerrando.\n");
            pthread_mutex_unlock(e->mutex);
            break;
        }
         pthread_mutex_unlock(e->mutex);

        PCB* p =  QUEUE_pop(e->fila);
        printf("POO\n");
        QUEUE_imprime(e->fila);
        if (!p) {
            printf("[ESCALONADOR] Nenhum processo retornado da fila. Continuando...\n");
            
            continue;
        }

        int pid = PCB_get_pid(p);
        printf("[ESCALONADOR] Processo %d selecionado.\n", pid);

        pthread_mutex_t* mtx = PCB_get_mutex(p);
        pthread_cond_t* cv = PCB_get_cond(p);

        pthread_mutex_lock(mtx);
        if (PCB_get_estado(p) == PRONTO) {
            PCB_set_estado(p, EXECUTANDO);
            printf("[ESCALONADOR] Processo %d setado para EXECUTANDO. Acordando threads.\n", pid);
            pthread_cond_broadcast(cv);
        } else {
            printf("[ESCALONADOR] Processo %d não está pronto (estado: %d). Ignorado.\n", pid, PCB_get_estado(p));
        }
        pthread_mutex_unlock(mtx);

        switch (e->politica) {
            case FCFS:
                printf("[ESCALONADOR] Aguardando finalização do processo %d (FCFS).\n", pid);
                fprintf(e->output_file,"[FCFS] Executando processo PID %d\n", pid);
                SCHEDULER_aguarda_finalizacao_processo(p,e);
                printf("[ESCALONADOR] Processo %d finalizado.\n", pid);
                fprintf(e->output_file,"[FCFS] Processo PID %d finalizado\n", pid);
                break;

            case ROUND_ROBIN:
                printf("[ESCALONADOR] (ROUND ROBIN) Executaria quantum para processo %d.\n", pid);
                SCHEDULER_executar_quantum(p,e,QUANTUM);
                
                // executar_quantum(p, e->quantum_ms);
                break;

            case PRIORIDADE:
                printf("[ESCALONADOR] (PRIORIDADE) Executaria processo %d com prioridade.\n", pid);
                // executar_quantum(p, e->quantum_ms);
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
   
    pthread_cond_signal(e->cv); // acorda a thread do escalonador
   
}
void SCHEDULER_executar_quantum(PCB* p, SCHEDULER* e, int quantum_ms) {
    int pid = PCB_get_pid(p);

    // Marca o início do quantum
    printf("[ROUND ROBIN] Executando processo PID %d por até %d ms\n", pid, quantum_ms);
   

    pthread_mutex_t* mtx = PCB_get_mutex(p);
   

    // Dorme por quantum_ms (simulação de execução)
    usleep(quantum_ms); 



   
    fprintf(e->output_file, "[RR] Executando processo PID %d com quantum %dms\n", pid, quantum_ms);

     pthread_mutex_lock(mtx);
    if (PCB_get_estado(p) == FINALIZADO) {
        // Processo finalizado dentro das threads
        e->processos_restantes--;
        if( e->processos_restantes ==0)
            e->todos_processos_chegaram =1;
        PCB_set_quantum_acabou(p);
        printf("[ROUND ROBIN] Processo %d finalizado durante o quantum.\n", pid);
        fprintf(e->output_file, "[RR] Processo PID %d finalizado\n", pid);
    } else {
        // Processo ainda tem tempo restante — preempcionar e reinserir na fila
        
        
        PCB_set_estado(p, PRONTO);
        PCB_set_quantum_acabou(p);
        pthread_cond_signal(e->cv);
        
        QUEUE_imprime(e->fila);
        QUEUE_push(e->fila, p);
        printf("PUSH\n");
        QUEUE_imprime(e->fila);
        

        printf("[ROUND ROBIN] Processo %d preemptado, reinserido na fila com %d ms restantes.\n",
               pid, PCB_get_remaining_time(p));
    }
    pthread_mutex_unlock(mtx);
    

}
int SHEDULER_get_escalonador_esperando(SCHEDULER* e){
    return e->escalonador_esperando;
}

pthread_mutex_t * SCHEDULER_get_mutex(SCHEDULER* e){
    return e->mutex;
}
pthread_cond_t* SCHEDULER_get_cv(SCHEDULER* e){
    return e->cv;
}
int SCHEDULER_get_escalonador_vazio(SCHEDULER* e){
    return e->escalonador_vazio;
}
