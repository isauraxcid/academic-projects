/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355 
 */

#include "memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>


void *allocate_dynamic_memory(int size) {
    void *ptr = malloc(size);

    if (ptr == NULL) {
        perror("malloc");
        return NULL;
    }

    memset(ptr, 0, size);
    return ptr;
}

void *create_shared_memory(char *name, int size) {
    if (name == NULL || size <= 0) {
        return NULL;
    }

    char nome_memoria[256];
    
#ifdef __APPLE__
    // Versão Mac: Cria um hash curto para evitar o erro "File name too long"
    unsigned long hash = 5381;
    int c;
    char *str = name;
    while ((c = *str++)) hash = ((hash << 5) + hash) + c;
    snprintf(nome_memoria, sizeof(nome_memoria), "/m_%lx", hash);
#else
    //Versão Oficial (Linux): Usa exatamente o nome passado pelo .h + UID
    int uid = (int)getuid();
    snprintf(nome_memoria, sizeof(nome_memoria), "/%s_%d", name, uid);
#endif

    int fd = shm_open(nome_memoria, O_CREAT | O_RDWR, 0666);
    if (fd < 0) {
        perror("shm_open");
        return NULL;
    }

    int trunc_result = ftruncate(fd, size);
    if (trunc_result < 0) {
        perror("ftruncate");
        close(fd);
        shm_unlink(nome_memoria);
        return NULL;
    }

    void *ptr = mmap(NULL, (size_t)size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        shm_unlink(nome_memoria);
        return NULL;
    }

    close(fd);
    memset(ptr, 0, (size_t)size);
    return ptr;
}

void destroy_shared_memory(char *name, void *ptr, int size) {
    if (ptr != NULL && size > 0) {
        munmap(ptr, (size_t)size);
    }

    if (name != NULL) {
        char nome_memoria[256];

#ifdef __APPLE__
        unsigned long hash = 5381;
        int c;
        char *str = name;
        while ((c = *str++)) hash = ((hash << 5) + hash) + c;
        snprintf(nome_memoria, sizeof(nome_memoria), "/m_%lx", hash);
#else
        int uid = (int)getuid();
        snprintf(nome_memoria, sizeof(nome_memoria), "/%s_%d", name, uid);
#endif

        shm_unlink(nome_memoria);
    }
}

int write_sensor_controller_buffer(struct ra_buffer *buffer, int buffer_size, const MeasurementInfo *m) {
    if (buffer == NULL || buffer->ptrs == NULL || buffer->buffer == NULL || m == NULL || buffer_size <= 0) {
        return 0;
    }

    for (int i = 0; i < buffer_size; i++) { //percorre o buffer 
        if (buffer->ptrs[i] == 0) { //se a posicao estiver vazia
            buffer->buffer[i] = *m; // mete a measure nessa posicao
            buffer->ptrs[i] = 1; //a posicao passa a estar preenchida
            return 1; //success
        }
    }
    return 0;
}

void read_sensor_controller_buffer(struct ra_buffer *buffer, int controller_id, int buffer_size, MeasurementInfo *m) {
    if (m == NULL) {
        return;
    }

    if (buffer == NULL || buffer->ptrs == NULL || buffer->buffer == NULL || buffer_size <= 0) {
        m->m_id = -1;
        return;
    }

    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 1) {
            MeasurementInfo candidato = buffer->buffer[i];

            if (candidato.sensor_id == controller_id) {
                *m = candidato;
                buffer->ptrs[i] = 0;
                return;
            }
        }
    }
    m->m_id = -1;
}

int write_controller_servers_buffer(struct ra_buffer *buffer, int buffer_size, const MeasurementInfo *m) {
    if (buffer == NULL || buffer->ptrs == NULL || buffer->buffer == NULL || m == NULL || buffer_size <= 0) return 0;

    for (int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 0) { //se a posicao estiver livre
            MeasurementInfo measure = *m; //measure criada para ocupar essa posicao
            measure.counter_servers = 1; //cada posicao pertence a um e um so server
            buffer->buffer[i] = measure; //essa posicao é preenchida pela measure
            buffer->ptrs[i] = 1; //posicao passa a estar ocupada
            return 1; //sucesso
        }
    }
    return 0;
}

void read_controller_servers_buffer(struct ra_buffer *buffer, int buffer_size, int expected_m_id, int expected_controller_id, MeasurementInfo *m) {
    if (m == NULL) {
        return;
    }
    if (buffer == NULL || buffer->ptrs == NULL || buffer->buffer == NULL || buffer_size <= 0) {
        m->m_id = -1;
        return;
    }
    for(int i = 0; i < buffer_size; i++) {
        if (buffer->ptrs[i] == 1) {
            MeasurementInfo candidato = buffer->buffer[i];

            if (candidato.m_id == expected_m_id && candidato.controller_id == expected_controller_id) {
                *m = candidato;

                buffer->buffer[i].counter_servers--;
                if (buffer->buffer[i].counter_servers <= 0) {
                    buffer->ptrs[i] = 0;
                }

                return;
            }
        }
    }
    m->m_id = -1;
}

void read_main_sensors_buffer(struct circ_buffer *buffer, int buffer_size, int expected_m_id, MeasurementInfo *req) {
    if (req == NULL) return;

    if (buffer == NULL || buffer->ptrs == NULL || buffer->buffer == NULL || buffer_size <= 0) {
        req->m_id = -1;
        return;
    }
    int in = buffer->ptrs->in;
    int out = buffer->ptrs->out;
    if (in == out) {
        req->m_id = -1; 
        return;
    }

    if (buffer->buffer[out].m_id != expected_m_id) {
        req->m_id = -1;
        return;
    }
    buffer->buffer[out].counter_sensors -= 1;
    
    *req = buffer->buffer[out];

    if (buffer->buffer[out].counter_sensors <= 0) {
        buffer->ptrs->out = (out + 1) % buffer_size;
    }
}

void deallocate_dynamic_memory(void *ptr) {
    if (ptr == NULL) {
        return;
    }
    free(ptr);
}

int write_main_sensors_buffer(struct circ_buffer *buffer, int buffer_size, const MeasurementInfo *req) {
    if (buffer == NULL || buffer->ptrs == NULL || buffer->buffer == NULL || req == NULL || buffer_size <= 0) {
        return 0;
    }

    int in = buffer->ptrs->in; //posicao onde se mete a proxima measure
    int out = buffer->ptrs->out; //next posicao a ser lida
    int next_in = (in + 1) % buffer_size; //calcula a proxima posicao no circ buffer

    if (next_in == out) {
        return 0; //se o buffer estiver cheio, nao da
    }
    MeasurementInfo measure = *req; //measure do argumento
    measure.counter_sensors = 2; //num de sencosres que ainda tem de processar a measure
    buffer->buffer[in] = measure; //mete a measure no buffer
    buffer->ptrs->in = next_in; //update o indice
    return 1; //sucesso
}