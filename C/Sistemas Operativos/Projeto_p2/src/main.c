#include "main.h"
#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main_args(int argc, char *argv[], struct info_container *info) {
    if (argc != 4) {
        printf("Uso correto: ./SOestimate n_sensors n_servers buff_size\n");
        exit(1);
    }
    info->n_sensors = atoi(argv[1]);
    info->n_servers = atoi(argv[2]);
    info->buffers_size = atoi(argv[3]);
}

void create_dynamic_memory_structs(struct info_container *info, struct buffers *buffs) {
    info->sensors_pids = allocate_dynamic_memory(info->n_sensors * sizeof(int));
    info->controllers_pids = allocate_dynamic_memory(info->n_sensors * sizeof(int));
    info->servers_pids = allocate_dynamic_memory(info->n_servers * sizeof(int));
    
    buffs->buff_main_sensors = allocate_dynamic_memory(sizeof(struct circ_buffer));
    buffs->buff_sensors_controllers = allocate_dynamic_memory(sizeof(struct ra_buffer));
    buffs->buff_controllers_servers = allocate_dynamic_memory(sizeof(struct ra_buffer));
}

void create_shared_memory_structs(struct info_container *info, struct buffers *buffs) {
    info->num_generated_measurements = create_shared_memory(ID_SHM_SENSORS_GENERATED, info->n_sensors * sizeof(int));
    info->num_invalid_measurements = create_shared_memory(ID_SHM_SENSORS_INVALID, info->n_sensors * sizeof(int));
    info->num_estimates = create_shared_memory(ID_SHM_SERVERS_PROCESSED, info->n_servers * sizeof(int));
    info->total_measurements = create_shared_memory(ID_SHM_TOTAL_MEASUREMENTS, sizeof(int));
    info->terminate = create_shared_memory(ID_SHM_TERMINATE, sizeof(int));
    
    buffs->buff_main_sensors->ptrs = create_shared_memory(ID_SHM_MAIN_SENSORS_PTR, sizeof(struct pointers));
    buffs->buff_main_sensors->buffer = create_shared_memory(ID_SHM_MAIN_SENSORS_BUFFER, info->buffers_size * sizeof(MeasurementInfo));
    
    buffs->buff_sensors_controllers->ptrs = create_shared_memory(ID_SHM_SENSORS_CONTROLLERS_PTR, info->buffers_size * sizeof(int));
    buffs->buff_sensors_controllers->buffer = create_shared_memory(ID_SHM_SENSORS_CONTROLLERS_BUFFER, info->buffers_size * sizeof(MeasurementInfo));
    
    buffs->buff_controllers_servers->ptrs = create_shared_memory(ID_SHM_CONTROLLERS_SERVERS_VERSION, info->buffers_size * sizeof(int));
    buffs->buff_controllers_servers->buffer = create_shared_memory(ID_SHM_CONTROLLERS_SERVERS_LATEST, info->buffers_size * sizeof(MeasurementInfo));
}

void create_processes(struct info_container *info, struct buffers *buffs) {
    for (int i = 0; i < info->n_sensors; i++) {
        info->sensors_pids[i] = launch_process(SENSOR_PROCESS, i, info, buffs);
        info->controllers_pids[i] = launch_process(CONTROLLER_PROCESS, i, info, buffs);
    }
    for (int i = 0; i < info->n_servers; i++) {
        info->servers_pids[i] = launch_process(SERVER_PROCESS, i, info, buffs);
    }
}

    void help(void) {
    printf("\nOperacoes disponiveis:\n  measure - Solicita a criacao de uma ronda de medicoes.\n  read j  - Obtem a estimativa media global da ronda j (Fase 2).\n  stat    - Mostra o estado atual do sistema.\n  help    - Mostra a informacao de ajuda.\n  end     - Termina a execucao do sistema.\n");
}


void print_stat(struct info_container *info) {
    printf("\n** Estado do Sistema -**\n");
    printf("Configuracao: %d Sensores/Controladores, %d Servidores, Buffer Size: %d\n", 
           info->n_sensors, info->n_servers, info->buffers_size);
    printf("Total de romdas de medicao: %d\n", *(info->total_measurements));
    
    printf("\nEstatiticas dos Sensores:\n");
    for (int i = 0; i < info->n_sensors; i++) {
        printf("  Sensor %d (PID %d): %d geradas / %d invalidas\n", 
               i, info->sensors_pids[i], 
               info->num_generated_measurements[i], 
               info->num_invalid_measurements[i]);
    }
    
    printf("\nEstatiticas dos Servidores:\n");
    for (int i = 0; i < info->n_servers; i++) {
        printf("  Servidor %d (PID %d): %d estimativas calculadas\n", 
               i, info->servers_pids[i], info->num_estimates[i]);
    }
    printf("***************************\n\n");
}

void read_estimate(struct info_container *info, int m_id) {
    printf("A operacao 'read' estara na 2a Fase do projeto.\n");
}

void user_interaction(struct info_container *info, struct buffers *buffs) {
    char input_utilizador[128];
    int j;

    printf("\nSOestimate iniciado. Escreve 'help' se percisas de uma ajudinha \n");

    while (!*(info->terminate)) {
        printf("> ");
        if (fgets(input_utilizador, sizeof(input_utilizador), stdin) == NULL) break;
        input_utilizador[strcspn(input_utilizador, "\n")] = 0; 

        if (strcmp(input_utilizador, "measure") == 0) {
            *(info->total_measurements) += 1;
            MeasurementInfo req;
            req.state = REQUEST;
            req.m_id = *(info->total_measurements);
            req.sensor_id = -1;
            req.controller_id = -1;
            req.value = 0.0;
            req.counter_sensors = info->n_sensors;
            req.counter_servers = info->n_servers;

            if (write_main_sensors_buffer(buffs->buff_main_sensors, info->buffers_size, &req)) {
                printf("Pedido de medicao (ronda %d) enviado com sucesso.\n", req.m_id);
            } else {
                printf("Erro: Buffer cheio. Tenta novamente mais tarde.\n");
                *(info->total_measurements) -= 1; 
            }
        } 
        else if (strncmp(input_utilizador, "read", 4) == 0) {
            if (sscanf(input_utilizador, "read %d", &j) == 1) {
                read_estimate(info, j);
            } else {
                printf("Uso correto: read <numero_da_ronda>\n");
            }
        } 
        else if (strcmp(input_utilizador, "stat") == 0) {
            print_stat(info);
        } 
        else if (strcmp(input_utilizador, "help") == 0) {
            help();
        } 
        else if (strcmp(input_utilizador, "end") == 0) {
            end_execution(info, buffs);
        } 
        else if (strlen(input_utilizador) > 0) {
            printf("Comando invalido. Escreve 'help' se quseres uma ajuda.\n");
        }
    }
}

void wait_processes(struct info_container *info) {
    for (int i = 0; i < info->n_sensors; i++) {
        wait_process(info->sensors_pids[i]);
        wait_process(info->controllers_pids[i]);
    }
    for (int i = 0; i < info->n_servers; i++) {
        wait_process(info->servers_pids[i]);
    }
}

void write_final_statistics(struct info_container *info) {
    printf("\nA terminar execucao do SOestimate...\n");
    print_stat(info);
}

void end_execution(struct info_container *info, struct buffers *buffs) {
    *(info->terminate) = 1;
    write_final_statistics(info);
    wait_processes(info);
}

void destroy_dynamic_memory_structs(struct info_container *info, struct buffers *buffs) {
    deallocate_dynamic_memory(info->sensors_pids);
    deallocate_dynamic_memory(info->controllers_pids);
    deallocate_dynamic_memory(info->servers_pids);
    deallocate_dynamic_memory(buffs->buff_main_sensors);
    deallocate_dynamic_memory(buffs->buff_sensors_controllers);
    deallocate_dynamic_memory(buffs->buff_controllers_servers);
}

void destroy_shared_memory_structs(struct info_container *info, struct buffers *buffs) {
    destroy_shared_memory(ID_SHM_SENSORS_GENERATED, info->num_generated_measurements, info->n_sensors * sizeof(int));
    destroy_shared_memory(ID_SHM_SENSORS_INVALID, info->num_invalid_measurements, info->n_sensors * sizeof(int));
    destroy_shared_memory(ID_SHM_SERVERS_PROCESSED, info->num_estimates, info->n_servers * sizeof(int));
    destroy_shared_memory(ID_SHM_TOTAL_MEASUREMENTS, info->total_measurements, sizeof(int));
    destroy_shared_memory(ID_SHM_TERMINATE, info->terminate, sizeof(int));

    destroy_shared_memory(ID_SHM_MAIN_SENSORS_PTR, buffs->buff_main_sensors->ptrs, sizeof(struct pointers));
    destroy_shared_memory(ID_SHM_MAIN_SENSORS_BUFFER, buffs->buff_main_sensors->buffer, info->buffers_size * sizeof(MeasurementInfo));

    destroy_shared_memory(ID_SHM_SENSORS_CONTROLLERS_PTR, buffs->buff_sensors_controllers->ptrs, info->buffers_size * sizeof(int));
    destroy_shared_memory(ID_SHM_SENSORS_CONTROLLERS_BUFFER, buffs->buff_sensors_controllers->buffer, info->buffers_size * sizeof(MeasurementInfo));

    destroy_shared_memory(ID_SHM_CONTROLLERS_SERVERS_VERSION, buffs->buff_controllers_servers->ptrs, info->buffers_size * sizeof(int));
    destroy_shared_memory(ID_SHM_CONTROLLERS_SERVERS_LATEST, buffs->buff_controllers_servers->buffer, info->buffers_size * sizeof(MeasurementInfo));
}

int main(int argc, char *argv[]) {
    struct info_container* info = allocate_dynamic_memory(sizeof(struct info_container));
    struct buffers* buffs = allocate_dynamic_memory(sizeof(struct buffers));

    main_args(argc, argv, info);
    create_dynamic_memory_structs(info, buffs);
    create_shared_memory_structs(info, buffs);
    create_processes(info, buffs);
    user_interaction(info, buffs);

    destroy_shared_memory_structs(info, buffs);
    destroy_dynamic_memory_structs(info, buffs);
    deallocate_dynamic_memory(info);
    deallocate_dynamic_memory(buffs);
    
    return 0;
}