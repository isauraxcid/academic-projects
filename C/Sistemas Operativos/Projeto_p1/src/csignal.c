/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355 
 */

#include "memory.h"
#include "ctime.h"
#include "main.h"
#include "csettings.h"
#include <unistd.h>
#include <signal.h>
#include <stdio.h>

/**
* invoca um alarme que apos o periodo definifo envia SIGALRM
*/
void armar(struct info_container *info) {
    if (info->period == 0) return; //se o periodo for 0 o alarme nao e ativado
    alarm(info->period); //alarme ativa no periodo definido
}

/**
* os processos filhos ignoram o sinal SIGINT (Crtl+C)
*/
void ignore_signals(void) {
    signal(SIGINT, SIG_IGN);
}