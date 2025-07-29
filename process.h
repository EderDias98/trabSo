#ifndef PROCESS_H
#define PROCESS_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

typedef enum {
    FCFS=1,
    ROUND_ROBIN,
    PRIORIDADE
} PoliticaEscalonamento;
// Definições para políticas de escalonamento


#define QUANTUM 500 // Quantum para Round Robin


// Tipos opacos
typedef struct PCB PCB;
typedef struct TCB TCB;
typedef enum { PRONTO, EXECUTANDO, FINALIZADO } EstadoProcesso;


void PCB_set_politica(PCB* p, PoliticaEscalonamento pe);
int PCB_get_tamanho();

void PCB_inicializa(PCB* processo ,int pid, int duracao_total, int prioridade, int num_threads, int tempo_chegada);
    

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
void PCB_le_processo(FILE *input_file, PCB* p, int pid);
int PCB_get_remaining_time(PCB* p);
#endif
