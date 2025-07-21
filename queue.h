#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

typedef struct FILA FILA;
// Função de criação
FILA* criaFila(int politica, PCB** processos) {
    FILA* fila = malloc(sizeof(FILA));
    fila->politica = politica;
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;
    fila->quantum = 500;  // valor padrão
    fila->processos = processos;
    return fila;
}

// Definir quantum para Round Robin
void set_quantum(FILA* fila, int quantum);
 

// Libera a fila
void destruir_fila(FILA* fila);
 
// Insere no fim (FCFS ou RR) ou ordenado (Prioridade)
void enfileirar(FILA* fila, PCB* processo);

   
// Remove do início (todos os algoritmos)
PCB* desenfileirar(FILA* fila);


// Verifica se está vazia
int fila_vazia(FILA* fila);


#endif