/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355 
 */
#include "memory.h"
#include "ctime.h"

/**
* invoca um alarme que apos o periodo definifo envia SIGALRM
*/
void armar(struct info_container *info);

/**
* os processos filhos ignoram o sinal SIGINT (Crtl+C)
*/
void ignore_signals(void);
