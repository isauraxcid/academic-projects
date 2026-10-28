#ifndef CSETTINGS_H_GUARD
#define CSETTINGS_H_GUARD

#include "main.h"

/* Ler os 3 int de args.txt: n_sensors, n_servers, buffers_size.
   Retorna 1 se correu bem, 0 se deu erro. */
int read_args_file(const char *filename, struct info_container *info);

/* Lê 3 linhas de settings.txt: log_filename, statistics_filename, period.
   Retorna 1 se correu bem, 0 se deu erro. */
int read_settings_file(const char *filename, struct info_container *info);

#endif