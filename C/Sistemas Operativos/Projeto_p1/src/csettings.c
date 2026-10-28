/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355 
 */

#include "csettings.h"
#include <stdio.h>
#include <string.h>

/* Ler os 3 int de args.txt: n_sensors, n_servers, buffers_size.
   Retorna 1 se correu bem, 0 se deu erro. */
int read_args_file(const char *nome_ficheiro, struct info_container *info)
{
    FILE *ficheiro = NULL;
    int num_lidos = 0;

    if (nome_ficheiro == NULL || info == NULL)
    {
        return 0;
    }

    // obter ponteiro para ficheiro
    ficheiro = fopen(nome_ficheiro, "r");

    if (ficheiro == NULL)
    {
        return 0;
    }

    num_lidos = fscanf(ficheiro, "%d %d %d",
                       &info->n_sensors,
                       &info->n_servers,
                       &info->buffers_size);

    fclose(ficheiro);

    if (num_lidos != 3)
    {
        return 0;
    }
    if (info->n_sensors <= 0 || info->n_servers <= 0 || info->buffers_size <= 0)
    {
        return 0;
    }

    return 1;
}

/* Lê 3 linhas de settings.txt: log_filename, statistics_filename, period.
   Retorna 1 se correu bem, 0 se deu erro. */
int read_settings_file(const char *nome_ficheiro, struct info_container *info)
{
    FILE *ficheiro = NULL;
    char linha_periodo[64];

    if (nome_ficheiro == NULL || info == NULL)
    {
        return 0;
    }

    ficheiro = fopen(nome_ficheiro, "r");
    if (ficheiro == NULL)
    {
        return 0;
    }

    /* ler nome do ficheiro de log */
    if (fgets(info->log_filename, MAX_FILENAME, ficheiro) == NULL)
    {
        fclose(ficheiro);
        return 0;
    }

    /* ler nome do ficheiro de estatisticas */
    if (fgets(info->statistics_filename, MAX_FILENAME, ficheiro) == NULL)
    {
        fclose(ficheiro);
        return 0;
    }

    /*ler periodo como string (para converter depois)*/
    if (fgets(linha_periodo, sizeof(linha_periodo), ficheiro) == NULL)
    {
        fclose(ficheiro);
        return 0;
    }

    fclose(ficheiro);

    info->log_filename[strcspn(info->log_filename, "\n")] = '\0';
    info->statistics_filename[strcspn(info->statistics_filename, "\n")] = '\0';
    linha_periodo[strcspn(linha_periodo, "\n")] = '\0';

    if (sscanf(linha_periodo, "%u", &info->period) != 1 || info->period == 0)
    {
        return 0;
    }

    return 1;
}
