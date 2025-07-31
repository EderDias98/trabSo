#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <pthread.h>
#include "process.h" // Define PCB
#include "queue.h"

typedef struct SCHEDULER SCHEDULER;

SCHEDULER* SCHEDULER_cria(PoliticaEscalonamento politica, int quantum_ms,FILA* f, int quantidade_processos, FILE* output_file );
 

PCB* SCHEDULER_seleciona_proximo_processo(SCHEDULER* e);
    


void* SCHEDULER_thread(void* arg);

    

void SCHEDULER_aguarda_finalizacao_processo(PCB* pcb,SCHEDULER* e);
 

void  SCHEDULER_notifica_novo_processo(SCHEDULER* e);
void SCHEDULER_executar_quantum(PCB* p, SCHEDULER* e, int quantum_ms);
int SHEDULER_get_escalonador_esperando(SCHEDULER* e);

pthread_mutex_t * SCHEDULER_get_mutex(SCHEDULER* e);
pthread_cond_t* SCHEDULER_get_cv(SCHEDULER* e);
#endif