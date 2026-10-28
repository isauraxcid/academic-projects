/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355 
 */

#include "clog.h"
#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <stdlib.h>

/*Abre o ficheiro em modo "w" (write) que automaticamente apaga o conteudo.
  Todas as execuções começam com um log vazio.*/
void log_init(const char *filename) {
    if (filename == NULL) return;
    FILE *f = fopen(filename, "w");
    if (f != NULL) fclose(f);
}

/*Escreve uma linha no ficheiro protegida por semáforo (mutex).
 *sem wait tenta decrementar o semáforo. Se estiver a 0, bloqueia até outro fazer sem_post.
 *info->sems->log_mutex é o semáforo
 *message é a string já formatada que é escrita. */
void log_write(struct info_container * info, const char *message) {
    if (info == NULL || message == NULL) return;
    if (info->sems == NULL || info->sems->log_mutex == NULL) return;

    //pede permissao para escrever(bloqueia caso outro processo esteja a escrever)
    sem_wait(info->sems->log_mutex);

    //abre em modo append para acrescentar a linha
    FILE *f = fopen(info->log_filename, "a");
    if (f != NULL) {
        fprintf(f, "%s\n", message);
        fclose(f);
    }
    //acorda algum semaforo em espera
    sem_post(info->sems->log_mutex);
}

/* Le o ficheiro e procura a ronda j, extrai a estimativa.
 * Sem semáforo porque ler é tolerável enquanto outro escreve. */
double log_read(const char *filename, int ronda_j) {
    if (filename == NULL || ronda_j < 1) return -1.0;

    FILE *f = fopen(filename, "r");
    if (f == NULL) return -1.0;

    char linha[256];
    int mid;
    double val;
    char token[64];

    //ler linha a linha
    while (fgets(linha, sizeof(linha), f) != NULL) {
        //tenta extrair numero ronda_id <n> = <valor>
        if (sscanf(linha, "ronda_id %d = %lf", &mid, &val) == 2) {
            if (mid == ronda_j) {
                fclose(f);
                return val;
            }
        }
        //se for ronda_id <n> = NULL
        else if (sscanf(linha, "ronda_id %d =%63s", &mid, token) == 2) {
            if (mid == ronda_j) {
                fclose(f);
                return -1.0; // encontra mas é NULL
            }
        }
    }
    fclose(f);
    return -1.0;
}