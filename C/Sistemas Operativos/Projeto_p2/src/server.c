#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int execute_server(int server_id, struct info_container *info, struct buffers *buffs) {
    int expected_m_id = 1;
    int *received_flags = allocate_dynamic_memory(info->n_sensors * sizeof(int));
    int controllers_received = 0;
    double sum = 0.0;
    int valid_count = 0;
    MeasurementInfo m;

    while (!*(info->terminate)) {
        for (int i = 0; i < info->n_sensors; i++) {
            if (received_flags[i] == 0) {
                read_expected_cycle_measurement(&m, expected_m_id, received_flags, info, buffs);
                if (m.m_id != -1 && m.controller_id == i) {
                    received_flags[i] = 1;
                    controllers_received++;
                    
                    if (m.state == VALID) {
                        sum += m.value;
                        valid_count++;
                    }
                }
            }
        }

        if (controllers_received == info->n_sensors) {
            double estimate = valid_count > 0 ? (sum / valid_count) : 0.0;
            server_print_estimate(server_id, expected_m_id, estimate, valid_count);
            info->num_estimates[server_id]++;
            
            expected_m_id++;
            controllers_received = 0;
            sum = 0.0;
            valid_count = 0;
            for (int i = 0; i < info->n_sensors; i++) received_flags[i] = 0;
        }
        usleep(10000);
    }
    deallocate_dynamic_memory(received_flags);
    return 0;
}

void read_expected_cycle_measurement(MeasurementInfo *m, int expected_m_id, const int *received_flags, struct info_container *info, struct buffers *buffs) {
    for (int i = 0; i < info->n_sensors; i++) {
        if (received_flags[i] == 0) {
            read_controller_servers_buffer(buffs->buff_controllers_servers, info->buffers_size, expected_m_id, i, m);
            if (m->m_id != -1) return;
        }
    }
    m->m_id = -1;
}

int server_process_measurement(MeasurementInfo *m, int server_id, struct info_container *info, double *estimate, int *valid_count) {
    return 0; 
}

void server_print_estimate(int server_id, int m_id, double estimate, int valid_count) {
    if (valid_count > 0) {
        printf("[Server %d] Estimate for m_id %d: %f\n", server_id, m_id, estimate);
    } else {
        printf("[Server %d] Estimate for m_id %d: NULL (No valid measurements)\n", server_id, m_id);
    }
}