#include <stdlib.h>
#include "queue.h"



struct FILA {
    PoliticaEscalonamento politica;
    PCB** processos;
    int inicio;
    int fim;
    int tamanho;
    int quantum;
    int max;  // Usado no Round Robin
};

// Função de criação
FILA* QUEUE_cria(int politica, int n_processes) {
    FILA* fila = malloc(sizeof(FILA));
    fila->politica = politica;
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;
    fila->processos = malloc (sizeof(PCB*)*n_processes);
    fila->max = n_processes;
    return fila;
}



// Libera a fila
void QUEUE_libera(FILA* fila) {
    if(!fila) return;
    free(fila->processos);
    free(fila);
}

// Insere no fim (FCFS ou RR) ou ordenado (Prioridade)
void QUEUE_push(FILA* fila, PCB* processo) {
    if (fila->tamanho >= fila->max) return;

    if (fila->politica == PRIORIDADE) {
        int i = fila->fim;
        while (i != fila->inicio) {
            int prev = (i - 1 + fila->max) % fila->max;
            if (PCB_get_prioridade(fila->processos[prev]) <= PCB_get_prioridade(processo)) break;
            fila->processos[i] = fila->processos[prev];
            i = prev;
        }
        fila->processos[i] = processo;
        fila->fim = (fila->fim + 1) % fila->max;
    } else {
        fila->processos[fila->fim] = processo;
        fila->fim = (fila->fim + 1) % fila->max;
    }

    fila->tamanho++;
}

// Remove do início (todos os algoritmos)
PCB* QUEUE_pop(FILA* fila) {
    if (fila->tamanho == 0) return NULL;

    PCB* proc = fila->processos[fila->inicio];
    fila->inicio = (fila->inicio + 1) % fila->max;
    fila->tamanho--;

    return proc;
}

// Verifica se está vazia
int  QUEUE_vazia(FILA* fila) {
    return fila->tamanho == 0;
}

void QUEUE_imprime(FILA* fila) {
    if (!fila) {
        printf("[FILA] (nula)\n");
        return;
    }

    printf("[FILA] inicio = %d, fim = %d, tamanho = %d\n", fila->inicio, fila->fim, fila->tamanho);

    if (fila->tamanho == 0) {
        printf("[FILA] (vazia)\n");
        return;
    }

    printf("[FILA] PIDs: ");
    for (int i = 0; i < fila->tamanho; i++) {
        int idx = (fila->inicio + i) % fila->max;
        printf("%d ", PCB_get_pid(fila->processos[idx]));
    }
    printf("\n");
}