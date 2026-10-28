/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355 
 */

#include "ctime.h"
#include <time.h>

/**
* devolve o instante de tempo atual
*/
struct timespec now_time () 
{
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now;
}

/**
* atualiza os timestamps do processo respetivo
*/
void update_timestamp (struct timestamps *time, enum processo processo) 
{
    switch (processo) {
        case MAIN:
            time->t_main = now_time();
            break;
        case SENSOR:
            time->t_sensor = now_time();
            break;
        case CONTROLLER:
            time->t_controller = now_time();
            break;
        case SERVER:
            time->t_server = now_time();
            break;
        default:
            break;
    }
}

/**
*Calcula a duracao do measurement no sistema
*/
double elapsed_time (struct timestamps *time) {
    return ((time->t_server.tv_sec + (time->t_server.tv_nsec/1000000000.0)) - (time->t_main.tv_sec + (time->t_main.tv_nsec/1000000000.0)));
}