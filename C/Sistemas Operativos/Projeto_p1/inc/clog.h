#ifndef CLOG_H_GUARD
#define CLOG_H_GUARD

#include "main.h"

/*Inicializa o ficheiro de log
  Limpa o ficheiro se já existir.
  Funçao chamada uma vez no arranque do programa. */
void log_init(const char *filename);

/* Escreve uma linha no log protegida por semáforo (mutex).
   A linha (message) já deve estar formatada antes de chamar. */
void log_write(struct info_container *info, const char *message);

/* Procura uma ronda no ficheiro e devolve a estimativa.
   Retorna -1.0 se não encontrar ou for NULL. */
double log_read(const char *filename, int ronda_j);

#endif