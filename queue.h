#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

typedef struct FILA FILA;

// Função de criação
FILA* QUEUE_cria(int politica, int n_processes);




// Libera a fila
void QUEUE_libera(FILA* fila);


// Insere no fim (FCFS ou RR) ou ordenado (Prioridade)
void QUEUE_push(FILA* fila, PCB* processo);


// Remove do início (todos os algoritmos)
PCB* QUEUE_pop(FILA* fila);
  

// Verifica se está vazia
int  QUEUE_vazia(FILA* fila);
void QUEUE_imprime(FILA* fila);

#endif