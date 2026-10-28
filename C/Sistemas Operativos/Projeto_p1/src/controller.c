/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355 
 */

#include <stdlib.h>
#include <unistd.h>
#include "memory.h"
#include "main.h"
#include "controller.h"
#include "random_measurement.h"
#include "ctime.h"
#include "csignal.h"
#include <stdio.h>

/*
 * Function executed by one controller process.
 * controller_id identifies which controller is running.
 * info provides shared flags and statistics.
 * buffs provides access to shared communication buffers.
 */
int execute_controller(int controller_id, struct info_container *info, struct buffers *buffs) {
    ignore_signals();
    while (!*(info->terminate)) {
        MeasurementInfo m;
        controller_receive_measurement(&m, controller_id, info, buffs);

        if (m.m_id == -1) {
            continue;
        }

        controller_process_measurement(&m, controller_id, info);
        controller_send_measurement(&m, info, buffs);
    }
    return 0;
}

/*
 * Function that reads one measurement for this controller from
 * the sensor->controller shared buffer.
 * m receives the read measurement (or an empty marker when no data exists).
 * controller_id identifies which paired sensor/controller message to read.
 * info provides runtime configuration and terminate flag.
 * buffs provides access to shared buffers.
 */
void controller_receive_measurement(MeasurementInfo *m, int controller_id, struct info_container *info, struct buffers *buffs) {
    sem_wait(info->sems->sensors_controllers->unread);
    sem_wait(info->sems->sensors_controllers->mutex);
    
    read_sensor_controller_buffer(buffs->buff_sensors_controllers, controller_id, info->buffers_size, m);

    sem_post(info->sems->sensors_controllers->mutex);
    sem_post(info->sems->sensors_controllers->free_space);
}

/*
 * Function that validates a measurement value and updates statistics.
 * m is the measurement to validate, and its state field is updated.
 * controller_id identifies which controller is validating the measurement.
 * info stores the shared invalid-measurement counters updated by the controller.
 */
void controller_process_measurement(MeasurementInfo *m, int controller_id, struct info_container *info) {
    m->controller_id = controller_id;
    if (is_valid_measurement(m->value)) m->state = VALID;
    else {
        m->state = INVALID;
        info->num_invalid_measurements[controller_id]++;
    }
    update_timestamp(&m->change_time, CONTROLLER);
}

/*
 * Function that sends one processed measurement to the shared controller->server buffer.
 * m is the measurement to publish.
 * info provides the shared buffer size and the termination flag.
 * buffs provides access to shared buffers.
 */
void controller_send_measurement(MeasurementInfo *m, struct info_container *info, struct buffers *buffs) {
    sem_wait(info->sems->controllers_servers->free_space);
    sem_wait(info->sems->controllers_servers->mutex);

    for (int i = 0; i < info->n_servers; i++) {
        write_controller_servers_buffer(buffs->buff_controllers_servers, info->buffers_size, m);
        sem_post(info->sems->controllers_servers->unread);
    }
    sem_post(info->sems->controllers_servers->mutex);
    sem_post(info->sems->controllers_servers->unread);
}