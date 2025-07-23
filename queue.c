#include <stdlib.h>
#include "queue.h"

#define MAX 1024

struct FILA {
    PoliticaEscalonamento politica;
    PCB** processos;
    int inicio;
    int fim;
    int tamanho;
    int quantum;  // Usado no Round Robin
};

// Função de criação
FILA* QUEUE_cria(int politica, PCB** processos) {
    FILA* fila = malloc(sizeof(FILA));
    fila->politica = politica;
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;
    fila->processos = processos;
    return fila;
}



// Libera a fila
void QUEUE_libera(FILA* fila) {
    if(!fila) return;
    free(fila);
}

// Insere no fim (FCFS ou RR) ou ordenado (Prioridade)
void QUEUE_push(FILA* fila, PCB* processo) {
    if (fila->tamanho >= MAX) return;

    if (fila->politica == PRIORIDADE) {
        int i = fila->fim;
        while (i != fila->inicio) {
            int prev = (i - 1 + MAX) % MAX;
            if (PCB_get_prioridade(fila->processos[prev]) <= PCB_get_prioridade(processo)) break;
            fila->processos[i] = fila->processos[prev];
            i = prev;
        }
        fila->processos[i] = processo;
        fila->fim = (fila->fim + 1) % MAX;
    } else {
        fila->processos[fila->fim] = processo;
        fila->fim = (fila->fim + 1) % MAX;
    }

    fila->tamanho++;
}

// Remove do início (todos os algoritmos)
PCB* QUEUE_pop(FILA* fila) {
    if (fila->tamanho == 0) return NULL;

    PCB* proc = fila->processos[fila->inicio];
    fila->inicio = (fila->inicio + 1) % MAX;
    fila->tamanho--;

    return proc;
}

// Verifica se está vazia
int  QUEUE_vazia(FILA* fila) {
    return fila->tamanho == 0;
}
