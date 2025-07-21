#include <stdlib.h>
#include "queue.h"

#define MAX 1024

struct FILA {
    int politica;
    PCB** processos;
    int inicio;
    int fim;
    int tamanho;
    int quantum;  // Usado no Round Robin
};

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
void set_quantum(FILA* fila, int quantum) {
    fila->quantum = quantum;
}

// Libera a fila
void destruir_fila(FILA* fila) {
    free(fila);
}

// Insere no fim (FCFS ou RR) ou ordenado (Prioridade)
void enfileirar(FILA* fila, PCB* processo) {
    if (fila->tamanho >= MAX) return;

    if (fila->politica == 3) {
        int i = fila->fim;
        while (i != fila->inicio) {
            int prev = (i - 1 + MAX) % MAX;
            if (PcbGetPrioridade(fila->processos[prev]) <= PcbGetPrioridade(processo)) break;
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
PCB* desenfileirar(FILA* fila) {
    if (fila->tamanho == 0) return NULL;

    PCB* proc = fila->processos[fila->inicio];
    fila->inicio = (fila->inicio + 1) % MAX;
    fila->tamanho--;

    return proc;
}

// Verifica se está vazia
int fila_vazia(FILA* fila) {
    return fila->tamanho == 0;
}
