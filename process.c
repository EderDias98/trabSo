#include "process.h"

// Definição dos estados possíveis do p

struct PCB
{

    int pid;               // ID único do p
    int duracao_total;     // duração total do p (em ms)
    int tempo_restante;    // quanto falta para o p acabar (em ms)
    int prioridade;        // prioridade do p (não usada ainda)
    int num_threads;       // quantas threads pertencem a este p
    int tempo_chegada;     // tempo (ms) para o p "chegar" no sistema
    EstadoProcesso estado; // estado atual do p

    pthread_mutex_t *mutex; // trava para evitar acesso concorrente
    pthread_cond_t *cv;     // variável de condição para sincronizar as threads
    pthread_t *threads_ids; // array com os IDs das threads do p
};

struct TCB
{
    PCB *pcb;
    int indice_thread;
};

#define TEMPO_EXECUCAO_THREAD 500 // simula 500ms de execução

int PCB_get_tamanho()
{
    return sizeof(PCB);
}

void PCB_le_processo(FILE *input_file, PCB* p, int pid)
{
    int duration, priority, num_threads, arrival_time;

    fscanf(input_file, "%d", &duration);
    fscanf(input_file, "%d", &priority);
    fscanf(input_file, "%d", &num_threads);
    fscanf(input_file, "%d", &arrival_time);

    // Inicializar PCB (função a ser implementada)
    PCB_inicializa( p, pid, duration, priority, num_threads, arrival_time);
}

void PCB_inicializa(PCB *p, int pid, int duracao_total, int prioridade, int num_threads, int tempo_chegada)
{

    if (p == NULL)
    {
        perror("Erro ao alocar memória para o p");
        exit(1);
    }

    p->pid = pid;
    p->duracao_total = duracao_total;
    p->tempo_restante = duracao_total;
    p->prioridade = prioridade;
    p->num_threads = num_threads;
    p->tempo_chegada = tempo_chegada;
    p->estado = PRONTO;
    p->mutex = malloc(sizeof(pthread_mutex_t));
    p->cv = malloc(sizeof(pthread_cond_t));

    // Inicializa mutex e variável de condição
    pthread_mutex_init(p->mutex, NULL);

    pthread_cond_init(p->cv, NULL);

    // Aloca espaço para os IDs das threads
    p->threads_ids = malloc(sizeof(pthread_t) * num_threads);
}

void PCB_libera(PCB *p)
{
    if (p == NULL)
        return;

    pthread_mutex_destroy(p->mutex);
    pthread_cond_destroy(p->cv);
    free(p->threads_ids);
    free(p);
}

void *PCB_funcao_thread(void *arg)
{
    TCB *tcb = (TCB *)arg;
    PCB *pcb = tcb->pcb;
    int id = tcb->indice_thread;

    printf("[THREAD %d.%d] Iniciando...\n", pcb->pid, id);

    while (1)
    {
        pthread_mutex_lock(pcb->mutex);
        while (pcb->estado != EXECUTANDO && pcb->estado != FINALIZADO)
        {
            printf("[THREAD %d.%d] Esperando para executar. Estado atual: %d\n", pcb->pid, id, pcb->estado);
            pthread_cond_wait(pcb->cv, pcb->mutex);
        }

        if (pcb->estado == FINALIZADO)
        {
            printf("[THREAD %d.%d] Processo finalizado. Encerrando thread.\n", pcb->pid, id);
            pthread_mutex_unlock(pcb->mutex);
            break;
        }

        pthread_mutex_unlock(pcb->mutex);

        printf("[THREAD %d.%d] Executando por %d ms...\n", pcb->pid, id, TEMPO_EXECUCAO_THREAD);
        usleep(TEMPO_EXECUCAO_THREAD);

        pthread_mutex_lock(pcb->mutex);
        if (pcb->tempo_restante > 0)
        {
            pcb->tempo_restante -= TEMPO_EXECUCAO_THREAD;
            if (pcb->tempo_restante <= 0)
            {
                pcb->tempo_restante = 0;
                pcb->estado = FINALIZADO;

                printf("[THREAD %d.%d] Processo terminou a execução!\n", pcb->pid, id);
                pthread_cond_broadcast(pcb->cv);
            }
            else
            {
                printf("[THREAD %d.%d] Tempo restante: %d ms\n", pcb->pid, id, pcb->tempo_restante);
            }
        }
        pthread_mutex_unlock(pcb->mutex);
    }

    printf("[THREAD %d.%d] Encerrada.\n", pcb->pid, id);
    // free(tcb);
    return NULL;
}

int PCB_get_tempo_chegada(PCB *p)
{
    return p->tempo_chegada;
}

int PCB_get_prioridade(PCB *p)
{
    return p->prioridade;
}
void PCB_create_threads(PCB *pcb)
{

    for (int i = 0; i < pcb->num_threads; i++)
    {
        TCB *tcb = malloc(sizeof(TCB));

        tcb->pcb = pcb;
        tcb->indice_thread = i;

        pthread_create(&pcb->threads_ids[i], NULL, PCB_funcao_thread, (void *)tcb);
    }
}

void PCB_join_threads(PCB *p)
{
    if (!p || !p->threads_ids)
        return;

    for (int i = 0; i < p->num_threads; i++)
    {
        pthread_join(p->threads_ids[i], NULL);
    }
}
EstadoProcesso PCB_get_estado(PCB *p)
{
    return p->estado;
}

void PCB_set_estado(PCB *p, EstadoProcesso esp)
{
    p->estado = esp;
}

pthread_mutex_t *PCB_get_mutex(PCB *p)
{
    return p->mutex;
} // trava para evitar acesso concorrente

pthread_cond_t *PCB_get_cond(PCB *p)
{
    return p->cv;
}

int PCB_get_pid(PCB *p)
{
    return p->pid;
}