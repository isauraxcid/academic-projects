/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355
 */

#include "main.h"
#include "process.h"
#include "synchronization.h"
#include "csettings.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "clog.h"
#include "ctime.h"
#include "csignal.h"
#include "cstats.h"

struct info_container *global_info;
struct buffers *global_buffs;

void main_args(int argc, char *argv[], struct info_container *info)
{
    (void)argc;
    (void)argv;

    // le dos ficheiros de configuracao
    if (!read_args_file("src/args.txt", info))
    {
        printf("Erro: Falha ao ler args.txt\n");
        exit(1);
    }

    if (!read_settings_file("src/settings.txt", info))
    {
        printf("Erro: Falha ao ler settings.txt\n");
        exit(1);
    }
}

void create_dynamic_memory_structs(struct info_container *info, struct buffers *buffs)
{
    info->sensors_pids = allocate_dynamic_memory(info->n_sensors * sizeof(int));
    info->controllers_pids = allocate_dynamic_memory(info->n_sensors * sizeof(int));
    info->servers_pids = allocate_dynamic_memory(info->n_servers * sizeof(int));

    buffs->buff_main_sensors = allocate_dynamic_memory(sizeof(struct circ_buffer));
    buffs->buff_sensors_controllers = allocate_dynamic_memory(sizeof(struct ra_buffer));
    buffs->buff_controllers_servers = allocate_dynamic_memory(sizeof(struct ra_buffer));
}

void create_shared_memory_structs(struct info_container *info, struct buffers *buffs)
{
    info->num_generated_measurements = create_shared_memory(ID_SHM_SENSORS_GENERATED, info->n_sensors * sizeof(int));
    info->num_invalid_measurements = create_shared_memory(ID_SHM_SENSORS_INVALID, info->n_sensors * sizeof(int));
    info->num_estimates = create_shared_memory(ID_SHM_SERVERS_PROCESSED, info->n_servers * sizeof(int));
    info->total_measurements = create_shared_memory(ID_SHM_TOTAL_MEASUREMENTS, sizeof(int));
    info->terminate = create_shared_memory(ID_SHM_TERMINATE, sizeof(int));
    info->last_logged_id = create_shared_memory(ID_SHM_LAST_LOGGED_ID, sizeof(int));

    if (info->last_logged_id != NULL)
    {
        *(info->last_logged_id) = 0;
    }

    buffs->buff_main_sensors->ptrs = create_shared_memory(ID_SHM_MAIN_SENSORS_PTR, sizeof(struct pointers));
    buffs->buff_main_sensors->buffer = create_shared_memory(ID_SHM_MAIN_SENSORS_BUFFER, info->buffers_size * sizeof(MeasurementInfo));

    buffs->buff_sensors_controllers->ptrs = create_shared_memory(ID_SHM_SENSORS_CONTROLLERS_PTR, info->buffers_size * sizeof(int));
    buffs->buff_sensors_controllers->buffer = create_shared_memory(ID_SHM_SENSORS_CONTROLLERS_BUFFER, info->buffers_size * sizeof(MeasurementInfo));

    buffs->buff_controllers_servers->ptrs = create_shared_memory(ID_SHM_CONTROLLERS_SERVERS_VERSION, info->buffers_size * sizeof(int));
    buffs->buff_controllers_servers->buffer = create_shared_memory(ID_SHM_CONTROLLERS_SERVERS_LATEST, info->buffers_size * sizeof(MeasurementInfo));
}

void create_processes(struct info_container *info, struct buffers *buffs)
{
    for (int i = 0; i < info->n_sensors; i++)
    {
        info->sensors_pids[i] = launch_process(SENSOR_PROCESS, i, info, buffs);
        info->controllers_pids[i] = launch_process(CONTROLLER_PROCESS, i, info, buffs);
    }
    for (int i = 0; i < info->n_servers; i++)
    {
        info->servers_pids[i] = launch_process(SERVER_PROCESS, i, info, buffs);
    }
}

void help(void)
{
    printf("\nOperacoes disponiveis:\n  measure - Solicita a criacao de uma ronda de medicoes.\n  read j  - Obtem a estimativa media global da ronda j (Fase 2).\n  stat    - Mostra o estado atual do sistema.\n  help    - Mostra a informacao de ajuda.\n  end     - Termina a execucao do sistema.\n");
}

void print_stat(struct info_container *info)
{
    printf("\n** Estado do Sistema -**\n");
    printf("Configuracao: %d Sensores/Controladores, %d Servidores, Buffer Size: %d\n", info->n_sensors, info->n_servers, info->buffers_size);
    printf("Total de romdas de medicao: %d\n", *(info->total_measurements));

    printf("\nEstatiticas dos Sensores:\n");
    for (int i = 0; i < info->n_sensors; i++)
    {
        printf("  Sensor %d (PID %d): %d geradas / %d invalidas\n", i, info->sensors_pids[i], info->num_generated_measurements[i], info->num_invalid_measurements[i]);
    }

    printf("\nEstatiticas dos Servidores:\n");
    for (int i = 0; i < info->n_servers; i++)
    {
        printf("  Servidor %d (PID %d): %d estimativas calculadas\n", i, info->servers_pids[i], info->num_estimates[i]);
    }
    printf("***************************\n\n");
}

void read_estimate(struct info_container *info, int m_id)
{
    double estimativa;

    estimativa = log_read(info->log_filename, m_id);

    if (estimativa < 0.0)
    {
        printf("Ronda %d: estimativa indisponivel.\n", m_id);
    }
    else
    {
        printf("Ronda %d: estimativa = %.3f\n", m_id, estimativa);
    }
}

void user_interaction(struct info_container *info, struct buffers *buffs)
{
    char input_utilizador[128];
    int j;

    printf("\nSOestimate iniciado. Escreve 'help' se percisas de uma ajuda \n");

    while (!*(info->terminate))
    {
        printf("> ");
        if (fgets(input_utilizador, sizeof(input_utilizador), stdin) == NULL)
            break;
        input_utilizador[strcspn(input_utilizador, "\n")] = 0;

        if (strcmp(input_utilizador, "measure") == 0)
        {
            *(info->total_measurements) += 1;
            MeasurementInfo req;
            req.state = REQUEST;
            req.m_id = *(info->total_measurements);
            req.sensor_id = -1;
            req.controller_id = -1;
            req.value = 0.0;
            req.counter_sensors = info->n_sensors;
            req.counter_servers = info->n_servers;

            update_timestamp(&req.change_time, MAIN); //o timestamp inicial

            sem_wait(info->sems->main_sensors->free_space);
            sem_wait(info->sems->main_sensors->mutex);

            if (write_main_sensors_buffer(buffs->buff_main_sensors, info->buffers_size, &req))
            {
                printf("Pedido de medicao (ronda %d) enviado com sucesso.\n", req.m_id);
            }
            else
            {
                printf("Erro: Buffer cheio. Tenta novamente mais tarde.\n");
                *(info->total_measurements) -= 1;
            }

            sem_post(info->sems->main_sensors->mutex);
            for (int i = 0; i < info->n_sensors; i++) {
                sem_post(info->sems->main_sensors->unread);
            }
        }
        else if (strncmp(input_utilizador, "read", 4) == 0)
        {
            if (sscanf(input_utilizador, "read %d", &j) == 1)
            {
                read_estimate(info, j);
            }
            else
            {
                printf("Uso correto: read <numero_da_ronda>\n");
            }
        }
        else if (strcmp(input_utilizador, "stat") == 0)
        {
            print_stat(info);
        }
        else if (strcmp(input_utilizador, "help") == 0)
        {
            help();
        }
        else if (strcmp(input_utilizador, "end") == 0)
        {
            end_execution(info, buffs);
        }
        else if (strlen(input_utilizador) > 0)
        {
            printf("Comando invalido. Escreve 'help' se quseres uma ajuda.\n");
        }
    }
}

void wait_processes(struct info_container *info)
{
    for (int i = 0; i < info->n_sensors; i++)
    {
        wait_process(info->sensors_pids[i]);
        wait_process(info->controllers_pids[i]);
    }
    for (int i = 0; i < info->n_servers; i++)
    {
        wait_process(info->servers_pids[i]);
    }
}

void write_final_statistics(struct info_container *info)
{
    printf("\nA terminar execucao do SOestimate...\n");
    print_stat(info);
    write_stats(info); //grava as estatisticas no ficheiro no final
}

void end_execution(struct info_container *info, struct buffers *buffs)
{
    (void)buffs;
    // Proteger a flag de terminacao
    sem_wait(info->sems->terminate_mutex);
    *(info->terminate) = 1;
    sem_post(info->sems->terminate_mutex);

    //wake up de todos para para terminar
    wakeup_processes(info);
    wait_processes(info);
    write_final_statistics(info);
}

void destroy_dynamic_memory_structs(struct info_container *info, struct buffers *buffs)
{
    if (info->sensors_pids)
        deallocate_dynamic_memory(info->sensors_pids);
    if (info->controllers_pids)
        deallocate_dynamic_memory(info->controllers_pids);
    if (info->servers_pids)
        deallocate_dynamic_memory(info->servers_pids);
    if (buffs->buff_main_sensors)
        deallocate_dynamic_memory(buffs->buff_main_sensors);
    if (buffs->buff_sensors_controllers)
        deallocate_dynamic_memory(buffs->buff_sensors_controllers);
    if (buffs->buff_controllers_servers)
        deallocate_dynamic_memory(buffs->buff_controllers_servers);
}

void destroy_shared_memory_structs(struct info_container *info, struct buffers *buffs)
{
    if (info->num_generated_measurements)
        destroy_shared_memory(ID_SHM_SENSORS_GENERATED, info->num_generated_measurements, info->n_sensors * sizeof(int));
    if (info->num_invalid_measurements)
        destroy_shared_memory(ID_SHM_SENSORS_INVALID, info->num_invalid_measurements, info->n_sensors * sizeof(int));
    if (info->num_estimates)
        destroy_shared_memory(ID_SHM_SERVERS_PROCESSED, info->num_estimates, info->n_servers * sizeof(int));
    if (info->total_measurements)
        destroy_shared_memory(ID_SHM_TOTAL_MEASUREMENTS, info->total_measurements, sizeof(int));
    if (info->terminate)
        destroy_shared_memory(ID_SHM_TERMINATE, info->terminate, sizeof(int));

    //destruir a shm do info->last_logged_id
    if (info->last_logged_id)
        destroy_shared_memory(ID_SHM_LAST_LOGGED_ID, info->last_logged_id, sizeof(int));

    if (buffs->buff_main_sensors && buffs->buff_main_sensors->ptrs)
        destroy_shared_memory(ID_SHM_MAIN_SENSORS_PTR, buffs->buff_main_sensors->ptrs, sizeof(struct pointers));
    if (buffs->buff_main_sensors && buffs->buff_main_sensors->buffer)
        destroy_shared_memory(ID_SHM_MAIN_SENSORS_BUFFER, buffs->buff_main_sensors->buffer, info->buffers_size * sizeof(MeasurementInfo));

    if (buffs->buff_sensors_controllers && buffs->buff_sensors_controllers->ptrs)
        destroy_shared_memory(ID_SHM_SENSORS_CONTROLLERS_PTR, buffs->buff_sensors_controllers->ptrs, info->buffers_size * sizeof(int));
    if (buffs->buff_sensors_controllers && buffs->buff_sensors_controllers->buffer)
        destroy_shared_memory(ID_SHM_SENSORS_CONTROLLERS_BUFFER, buffs->buff_sensors_controllers->buffer, info->buffers_size * sizeof(MeasurementInfo));

    if (buffs->buff_controllers_servers && buffs->buff_controllers_servers->ptrs)
        destroy_shared_memory(ID_SHM_CONTROLLERS_SERVERS_VERSION, buffs->buff_controllers_servers->ptrs, info->buffers_size * sizeof(int));
    if (buffs->buff_controllers_servers && buffs->buff_controllers_servers->buffer)
        destroy_shared_memory(ID_SHM_CONTROLLERS_SERVERS_LATEST, buffs->buff_controllers_servers->buffer, info->buffers_size * sizeof(MeasurementInfo));
}

// executa quando o programa recebe SIGINT (Ctrl+C) e termina execução
static void handler(int sinal) {
    (void)sinal; // nao usamos, ignoramos
    end_execution(global_info, global_buffs);
}

//executa quando o alarme dispara (SIGALRM)
static void alarm_handler(int sinal) {
    (void)sinal; //nao usamos, mas cm a funcao recebe como argumento, queremos ignorar
    print_stat(global_info);
    armar(global_info);
}

int main(int argc, char *argv[])
{
    struct info_container *info = allocate_dynamic_memory(sizeof(struct info_container));
    struct buffers *buffs = allocate_dynamic_memory(sizeof(struct buffers));

    global_info = info;
    global_buffs = buffs;

    if (!info || !buffs)
    {
        printf("[ERRO ] Falha ao alocar as estruturas base.\n");
        if (info)
            deallocate_dynamic_memory(info);
        if (buffs)
            deallocate_dynamic_memory(buffs);
        return 1;
    }

    main_args(argc, argv, info);

    create_dynamic_memory_structs(info, buffs);
    if (!info->sensors_pids || !buffs->buff_main_sensors || !buffs->buff_sensors_controllers || !buffs->buff_controllers_servers)
    {
        printf("[ERRO ] Falha na alocacao de memoria dinamica. A iniciar limpeza...\n");
        destroy_dynamic_memory_structs(info, buffs);
        deallocate_dynamic_memory(info);
        deallocate_dynamic_memory(buffs);
        return 1;
    }

    create_shared_memory_structs(info, buffs);
    if (!info->terminate || !buffs->buff_main_sensors->ptrs || !buffs->buff_sensors_controllers->buffer)
    {
        printf("[ERRO] Falha na criacao de memoria partilhada. A iniciar limpeza...\n");
        destroy_shared_memory_structs(info, buffs);
        destroy_dynamic_memory_structs(info, buffs);
        deallocate_dynamic_memory(info);
        deallocate_dynamic_memory(buffs);
        return 1;
    }

    // cria a estrutura dos semaforos antes dos processos arrancarem
    info->sems = create_all_semaphores(info->buffers_size);
    if (info->sems == NULL)
    {
        printf("[ERRO] Falha na criacao dos semaforos. A iniciar limpeza...\n");
        destroy_shared_memory_structs(info, buffs);
        destroy_dynamic_memory_structs(info, buffs);
        deallocate_dynamic_memory(info);
        deallocate_dynamic_memory(buffs);
        return 1;
    }

    log_init(info->log_filename);

    //regista o Signal Handler para o CTRL+C
    signal(SIGINT, handler);
    signal(SIGALRM, alarm_handler);
    create_processes(info, buffs);

    //ativa o alarme periodico
    armar(info);
    user_interaction(info, buffs);

    // destroi os semaforos quando o programa termina
    destroy_all_semaphores(info->sems);

    destroy_shared_memory_structs(info, buffs);
    destroy_dynamic_memory_structs(info, buffs);
    deallocate_dynamic_memory(info);
    deallocate_dynamic_memory(buffs);

    return 0;
}