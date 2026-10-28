#include <stdlib.h>
#include <unistd.h>
#include "memory.h"
#include "main.h"
#include "random_measurement.h"

/* Function executed by one Sensor process.
 * sensor_id identifies which sensor is running.
 * info provides shared flags and statistics.
 * buffs provides access to shared communication buffers.
 *
 * Notice: when this function starts, seed_sensor_rng(sensor_id) must be
 * called to initialize that sensor's random number generator state.
 * Otherwise, different sensor processes may generate the same random
 * sequence after fork().
 */
int execute_sensor(int sensor_id, struct info_container *info, struct buffers *buffs) {
    seed_sensor_rng(sensor_id);
    int expected_m_id = 1;

    while (!*(info->terminate)) { //processa rondas até o sistema terminar
        MeasurementInfo req;
        sensor_receive_request(&req, expected_m_id, info, buffs);

        if (req.m_id == -1) continue; //se não há um request útil, voltar ao início do ciclo e tentar novamente

        sensor_process_request(&req, sensor_id, info); //gera a medição do sensor para esta ronda
        sensor_send_measurement(&req, info, buffs); //escreve a medição no buffer sensor->controller
        expected_m_id++; //espera a ronda seguinte
    }
    return 0;
}

/* Function that reads the next request from the main->sensors circular buffer.
 * req receives the MeasurementInfo request data, or m_id = -1 when no request exists.
 * info provides runtime configuration and terminate flag.
 * buffs provides access to shared buffers.
 */
void sensor_receive_request(MeasurementInfo *req, int expected_m_id, struct info_container *info, struct buffers *buffs) { 
    read_main_sensors_buffer(buffs->buff_main_sensors, info->buffers_size, expected_m_id, req); // lê o pedido da ronda esperada do buffer circ
}

/* Function that creates one measurement from a received request.
 * m receives the generated measurement data.
 * sensor_id identifies the source sensor.
 * info provides shared counters updated by the sensor.
 */
void sensor_process_request(MeasurementInfo *m, int sensor_id, struct info_container *info) {
    m->sensor_id = sensor_id; //atualiza o id do sensor
    m->value = get_measurement(); //gera o valor da medição
    m->state = MEASURED; //atualiza o estado de medição para MEASURED
    info->num_generated_measurements[sensor_id]++; //incrementa o número de medições geradas por este sensor
}

/* Function that sends one generated measurement to the sensor->controller buffer.
 * m is the measurement to send.
 * info provides buffer size and terminate flag.
 * buffs provides access to shared buffers.
 */
void sensor_send_measurement(MeasurementInfo *m, struct info_container *info, struct buffers *buffs) {
    int send = write_sensor_controller_buffer(buffs->buff_sensors_controllers, info->buffers_size, m);
    while (!*(info->terminate) && !send) {
        sleep(1); //se o buffer estiver cheio, espera 1 segundo e tenta novamente
        send = write_sensor_controller_buffer(buffs->buff_sensors_controllers, info->buffers_size, m);
    }
}