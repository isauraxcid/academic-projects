/*
 * Grupo 14
 * José Marques - 63777
 * Isaura Cid - 63761
 * Mariana Franco - 62355 
 */

#include "process.h"
#include "sensor.h"
#include "controller.h"
#include "server.h"

#include <unistd.h>   
#include <sys/wait.h> 
#include <stdlib.h>   
#include <stdio.h>    

int launch_process(enum process_type process_type, int process_id, struct info_container *info, struct buffers *buffs)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        int child_status = 1;

        if (process_type == SENSOR_PROCESS)
        {
            child_status = execute_sensor(process_id, info, buffs);
        }
        else if (process_type == CONTROLLER_PROCESS)
        {
            child_status = execute_controller(process_id, info, buffs);
        }
        else if (process_type == SERVER_PROCESS)
        {
            child_status = execute_server(process_id, info, buffs);
        }
        else
        {
            
            child_status = 1;
        }

        
        exit(child_status);
    }

    
    return (int)pid;
}

int wait_process(int process_id)
{
    if (process_id <= 0)
    {
        return -1;
    }

    int status = 0;
    pid_t result = waitpid((pid_t)process_id, &status, 0);

    if (result < 0)
    {
        perror("waitpid");
        return -1;
    }

    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status);
    }

    return -1;
}