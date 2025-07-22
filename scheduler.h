#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <pthread.h>
#include "process.h" // Define PCB
#include "queue.h"

typedef struct SCHEDULER SCHEDULER;

SCHEDULER* SCHEDULER_cria(PoliticaEscalonamento politica, int quantum_ms,FILA* f );
 

PCB* SCHEDULER_seleciona_proximo_processo(SCHEDULER* e);
    


void* SCHEDULER_thread(void* arg);

    

void SCHEDULER_aguarda_finalizacao_processo(PCB* pcb);
 

void  SCHEDULER_notifica_novo_processo(SCHEDULER* e);

#endif