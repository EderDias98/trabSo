#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <unistd.h>

#include "process.h"   // Estrutura PCB e funções relacionadas
#include "scheduler.h" // Funções do escalonador e fila pronta
#include "queue.h"


#define MAX_PROCESSES 100

long current_time_millis()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (long)(tv.tv_sec) * 1000 + (tv.tv_usec) / 1000;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(stderr, "Uso: %s arquivo_entrada\n", argv[0]);
        return 1;
    }

    // --- 1. Leitura dos dados de entrada ---
    FILE *input_file = fopen(argv[1], "r");
    FILE * output_file = fopen("log_execucao_minikernel.txt","w");
    if (!input_file)
    {
        perror("Erro ao abrir arquivo de entrada");
        return 1;
    }

    int n_processes;
    fscanf(input_file, "%d", &n_processes);

    // Alocar lista de PCBs
    PCB **pcb_list = malloc(sizeof(PCB *) * n_processes);
    for (int i = 0; i < n_processes; i++)
    {
        pcb_list[i] = malloc(PCB_get_tamanho());
    }

    // Variáveis temporárias para leitura
    
    for (int i = 0; i < n_processes; i++)
    {
        PCB_le_processo(input_file,pcb_list[i],i+1);
    }

    // Ler política de escalonamento
    int scheduler_type;
    fscanf(input_file, "%d", &scheduler_type);
    fclose(input_file);

    // --- 2. Inicialização do escalonador ---

    // --- 3. Inicialização da fila de prontos e variáveis globais ---
    FILA *f = QUEUE_cria(scheduler_type, n_processes);

    SCHEDULER *e = SCHEDULER_cria(scheduler_type, 500, f, n_processes,output_file);
    // --- 4. Criar thread escalonadora ---
    pthread_t scheduler_thread;
    pthread_create(&scheduler_thread, NULL, SCHEDULER_thread, e);

    // --- 5. Criar threads dos processos na hora certa ---
    // O ideal é criar uma thread que monitora o tempo e cria as threads dos processos
    // Simulação do tempo inicial
    long start_time_ms = current_time_millis();
    int processos_adicionados = 0;
    int *inserido = calloc(n_processes, sizeof(int));
    if (!inserido)
    {
        perror("calloc falhou");
        exit(EXIT_FAILURE);
    }

    while (processos_adicionados < n_processes)
    {
        long agora = current_time_millis() - start_time_ms;

        for (int i = 0; i < n_processes; i++)
        {
            
            int temp_chegada = PCB_get_tempo_chegada(pcb_list[i]);

            if (!inserido[i] && temp_chegada <= agora)
            {
                printf(">> Threads do processo PID %d criadas\n", PCB_get_pid(pcb_list[i]));
                
                inserido[i] = 1;

                PCB_create_threads(pcb_list[i]);
                QUEUE_push(f, pcb_list[i]);
                SCHEDULER_notifica_novo_processo(e);

                processos_adicionados++;
            }
        }

        usleep(1000); // 1ms
    }
    free(inserido);

    // --- 6. Esperar escalonador finalizar ---
    pthread_join(scheduler_thread, NULL);

    // --- 7. Esperar todas as threads dos processos finalizarem ---
    for (int i = 0; i < n_processes; i++)
    {
        PCB_join_threads(pcb_list[i]);
    }

    // --- 9. Liberar memória e finalizar ---
    for (int i = 0; i < n_processes; i++)
    {
        PCB_libera(pcb_list[i]);
    }

    free(pcb_list);
    fprintf(output_file,"Escalonador terminou execução de todos processos\n");
    fclose(output_file);
    return 0;
}
