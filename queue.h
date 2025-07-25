#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

typedef struct FILA FILA;
typedef enum {
    FCFS=1,
    ROUND_ROBIN,
    PRIORIDADE
} PoliticaEscalonamento;
// Função de criação
FILA* QUEUE_cria(int politica, PCB** processos);




// Libera a fila
void QUEUE_libera(FILA* fila);


// Insere no fim (FCFS ou RR) ou ordenado (Prioridade)
void QUEUE_push(FILA* fila, PCB* processo);


// Remove do início (todos os algoritmos)
PCB* QUEUE_pop(FILA* fila);
  

// Verifica se está vazia
int  QUEUE_vazia(FILA* fila);


#endif