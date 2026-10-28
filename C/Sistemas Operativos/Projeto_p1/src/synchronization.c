/*
 * Grupo 014
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355
 */

#include "synchronization.h"
#include "main.h" 
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

sem_t *create_semaphore(char *name, int value) {
    sem_unlink(name);
    
    sem_t *sem = sem_open(name, O_CREAT, 0666, value);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        return NULL;
    }
    return sem;
}

void destroy_semaphore(char *name, sem_t *sem) {
    if (sem != SEM_FAILED && sem != NULL) {
        sem_close(sem);
    }
    if (name != NULL) {
        sem_unlink(name);
    }
}

struct buffer_semaphores *create_buffer_semaphores(int value, char *unread_name, char *free_name, char *mutex_name) {
    struct buffer_semaphores *b_sems = malloc(sizeof(struct buffer_semaphores));
    if (b_sems == NULL) return NULL;
    
    b_sems->unread = create_semaphore(unread_name, 0);
    b_sems->free_space = create_semaphore(free_name, value);
    b_sems->mutex = create_semaphore(mutex_name, 1);

    return b_sems;
}

void destroy_buffer_semaphores(struct buffer_semaphores *sems, char *unread_name, char *free_name, char *mutex_name) {
    if (sems == NULL) return;
    
    destroy_semaphore(unread_name, sems->unread);
    destroy_semaphore(free_name, sems->free_space);
    destroy_semaphore(mutex_name, sems->mutex);
    
    free(sems);
}

struct semaphores *create_all_semaphores(unsigned value) {
    struct semaphores *sems = malloc(sizeof(struct semaphores));
    if (sems == NULL) return NULL;

    sems->main_sensors = create_buffer_semaphores((int)value, SEM_MAIN_SENSORS_UNREAD, SEM_MAIN_SENSORS_FREE, SEM_MAIN_SENSORS_MUTEX);
    sems->sensors_controllers = create_buffer_semaphores((int)value, SEM_SENSORS_CONTROLLERS_UNREAD, SEM_SENSORS_CONTROLLERS_FREE, SEM_SENSORS_CONTROLLERS_MUTEX);
    sems->controllers_servers = create_buffer_semaphores((int)value, SEM_CONTROLLERS_SERVERS_UNREAD, SEM_CONTROLLERS_SERVERS_FREE, SEM_CONTROLLERS_SERVERS_MUTEX);

    sems->terminate_mutex = create_semaphore(SEM_TERMINATE_MUTEX, 1);
    sems->log_mutex = create_semaphore(SEM_LOG_MUTEX, 1);

    return sems;
}

void destroy_all_semaphores(struct semaphores *sems) {
    if (sems == NULL) return;

    destroy_buffer_semaphores(sems->main_sensors, SEM_MAIN_SENSORS_UNREAD, SEM_MAIN_SENSORS_FREE, SEM_MAIN_SENSORS_MUTEX);
    destroy_buffer_semaphores(sems->sensors_controllers, SEM_SENSORS_CONTROLLERS_UNREAD, SEM_SENSORS_CONTROLLERS_FREE, SEM_SENSORS_CONTROLLERS_MUTEX);
    destroy_buffer_semaphores(sems->controllers_servers, SEM_CONTROLLERS_SERVERS_UNREAD, SEM_CONTROLLERS_SERVERS_FREE, SEM_CONTROLLERS_SERVERS_MUTEX);

    destroy_semaphore(SEM_TERMINATE_MUTEX, sems->terminate_mutex);
    destroy_semaphore(SEM_LOG_MUTEX, sems->log_mutex);

    free(sems);
}

void print_all_semaphores(struct semaphores *sems) {
    if (sems == NULL) return;
    
    printf("--- Estado da Sincronizacao ---\n");
    printf("Semaforos inicializados e alocados em memoria.\n");
    printf("-------------------------------\n");
}

void wakeup_processes(struct info_container *info) {
    if (info == NULL || info->sems == NULL) return;

    int total_procs = (info->n_sensors * 2) + info->n_servers; 

    for (int i = 0; i < total_procs + 5; i++) {
        sem_post(info->sems->main_sensors->unread);
        sem_post(info->sems->main_sensors->free_space);
        
        sem_post(info->sems->sensors_controllers->unread);
        sem_post(info->sems->sensors_controllers->free_space);
        
        sem_post(info->sems->controllers_servers->unread);
        sem_post(info->sems->controllers_servers->free_space);
    }
}