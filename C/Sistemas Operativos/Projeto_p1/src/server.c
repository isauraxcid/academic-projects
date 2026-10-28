/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355
 */

#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "clog.h"
#include "ctime.h"
#include "csignal.h"

int execute_server(int server_id, struct info_container *info, struct buffers *buffs)
{
    ignore_signals();
    int expected_m_id = 1;
    int *received_flags = allocate_dynamic_memory(info->n_sensors * sizeof(int));

    for (int i = 0; i < info->n_sensors; i++) {
          received_flags[i] = 0;
    }

    if (received_flags == NULL)
    {
        printf("[Servidor %d] Erro: Nao foi possivel alocar memoria para as flags.\n", server_id);
        return 1;
    }

    double estimate = 0.0;
    int valid_count = 0;
    MeasurementInfo m;

    while (!*(info->terminate))
    {

        sem_wait(info->sems->controllers_servers->unread);

        if (*(info->terminate))
        {
            sem_post(info->sems->controllers_servers->unread);
            break;
        }

        sem_wait(info->sems->controllers_servers->mutex);

        read_expected_cycle_measurement(&m, expected_m_id, received_flags, info, buffs);

        sem_post(info->sems->controllers_servers->mutex);

        if (m.m_id != -1)
        {
            sem_post(info->sems->controllers_servers->free_space);

            received_flags[m.controller_id] = 1;

            int is_ready = server_process_measurement(&m, server_id, info, &estimate, &valid_count);

            if (is_ready)
            {
                server_print_estimate(server_id, expected_m_id, estimate, valid_count);

                char message[256];

                if (valid_count > 0)
                {
                    snprintf(message, sizeof(message), "ronda_id %d = %.3f", expected_m_id, estimate);
                }
                else
                {
                    snprintf(message, sizeof(message), "ronda_id %d = NULL", expected_m_id);
                }

                log_write(info, message);

                expected_m_id++;
                valid_count = 0;
                for (int j = 0; j < info->n_sensors; j++)
                {
                    received_flags[j] = 0;
                }
            }
        }
        else
        {
            sem_post(info->sems->controllers_servers->unread);
        }
    }

    deallocate_dynamic_memory(received_flags);
    return 0;
}

void read_expected_cycle_measurement(MeasurementInfo *m, int expected_m_id, const int *received_flags, struct info_container *info, struct buffers *buffs)
{
    for (int i = 0; i < info->n_sensors; i++)
    {
        if (received_flags[i] == 0)
        {
            read_controller_servers_buffer(buffs->buff_controllers_servers, info->buffers_size, expected_m_id, i, m);
            if (m->m_id != -1)
                return;
        }
    }
    m->m_id = -1;
}

int server_process_measurement(MeasurementInfo *m, int server_id, struct info_container *info, double *estimate, int *valid_count)
{
    (void)server_id;
    static int controllers_received = 0;
    static double sum = 0.0;

    controllers_received++;

    if (m->state == VALID)
    {
        sum += m->value;
        (*valid_count)++;
    }
    update_timestamp(&m->change_time, SERVER);
    if (controllers_received == info->n_sensors)
    {
        *estimate = (*valid_count > 0) ? (sum / *valid_count) : 0.0;
        info->num_estimates[server_id]++;
        controllers_received = 0;
        sum = 0.0;
        return 1;
    }
    return 0;
}

void server_print_estimate(int server_id, int m_id, double estimate, int valid_count)
{
    if (valid_count > 0)
    {
        printf(" -> [Servidor %d] Estimativa calculada (Ronda %d): %.3f\n", server_id, m_id, estimate);
    }
    else
    {
        printf(" -> [Servidor %d] Estimativa (Ronda %d): NULL (Sem medicoes validas.)\n", server_id, m_id);
    }
}