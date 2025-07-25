#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Definições para políticas de escalonamento


#define QUANTUM 500 // Quantum para Round Robin


// Tipos opacos
typedef struct PCB PCB;
typedef struct TCB TCB;
typedef enum { PRONTO, EXECUTANDO, FINALIZADO } EstadoProcesso;

int PCB_get_tamanho();

PCB* PCB_inicializa(PCB* processo ,int pid, int duracao_total, int prioridade, int num_threads, int tempo_chegada);
    

void PCB_libera(PCB* processo);
 

void*  PCB_funcao_thread(void* arg);

     

int PCB_get_tempo_chegada(PCB* p);
  
int PCB_get_prioridade(PCB*p);
  
void PCB_create_threads(PCB* pcb);
 
void PCB_join_threads(PCB* p);
EstadoProcesso PCB_get_estado(PCB* p);
 

void PCB_set_estado(PCB* p, EstadoProcesso esp);
pthread_mutex_t* PCB_get_mutex(PCB* p);
  
pthread_cond_t* PCB_get_cond(PCB* p);

int PCB_get_pid(PCB* p);

#endif
