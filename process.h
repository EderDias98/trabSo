#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Definições para políticas de escalonamento
#define FCFS 1
#define ROUND_ROBIN 2
#define PRIORITY_PREEMPTIVE 3

#define QUANTUM 500 // Quantum para Round Robin


// Tipos opacos
typedef struct PCB PCB;
typedef struct TCB TCB;

int tamanhoPcb();
PCB* InicializaPCB(PCB* processo ,int pid, int duracao_total, int prioridade, int num_threads, int tempo_chegada);
void destruirPCB(PCB* processo);
int getPcbTempoChegada(PCB* p);
int PcbGetPrioridade(PCB*p);
#endif
