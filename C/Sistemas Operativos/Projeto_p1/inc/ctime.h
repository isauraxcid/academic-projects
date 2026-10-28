#ifndef CTIME_H_GUARD
#define CTIME_H_GUARD

#include <time.h>

struct timestamps {
    struct timespec t_main;
    struct timespec t_sensor;
    struct timespec t_controller;
    struct timespec t_server;
};

/**
 * enumerado usado para atualizar os timestamps do processo respetivo
 */
enum processo {MAIN, SENSOR, CONTROLLER, SERVER};

/**
* devolve o instante de tempo atual
*/
struct timespec now_time ();

/**
* atualiza os timestamps do processo respetivo
*/
void update_timestamp (struct timestamps *time, enum processo processo);

/**
* Calcula a duracao do measurement no sistema
*/
double elapsed_time (struct timestamps *time);

#endif