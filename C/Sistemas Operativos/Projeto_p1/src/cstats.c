/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355 
 */

#include "main.h"
#include <stdio.h>
#include <unistd.h>

/**
 * formata o texto a ser escrito no ficheiro stats.txt
 */
void write_stats(struct info_container *info) {
    FILE *stats = fopen(info->statistics_filename, "w");

    if (stats == NULL) return;

    fprintf(stats, "===== FINAL STATISTICS =====\n");
    fprintf(stats, "Main PID: %d\n", getpid());
    fprintf(stats, "n_sensors=%d, n_servers=%d, buffers_size=%d\n", info->n_sensors, info->n_servers, info->buffers_size);
    fprintf(stats, "total_measurements=%d\n", *(info->total_measurements));
    fprintf(stats, "Sensors:\n");

    for(int sensor = 0; sensor < info->n_sensors; sensor++) {
        fprintf(stats, "    Sensor %d (PID %d): generated=%d\n", sensor, info->sensors_pids[sensor], info->num_generated_measurements[sensor]);
    }

    fprintf(stats, "Controllers:\n");

    for(int cont = 0; cont < info->n_sensors; cont++) {
        fprintf(stats, "controller=%d pid=%d\n", cont, info->controllers_pids[cont]);
    }

    fprintf(stats, "Servers:\n");

    for(int server = 0; server < info->n_servers; server++) {
        fprintf(stats, "    Server %d (PID %d): estimates=%d\n", server, info->servers_pids[server], info->num_estimates[server]);
    }

    fprintf(stats, "Last logged ID: %d\n", *(info->last_logged_id));
    fprintf(stats, "============================\n");
    fclose(stats);
}