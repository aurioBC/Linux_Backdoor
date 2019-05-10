// cmd_args.h
#ifndef CMD_ARGS_H
#define CMD_ARGS_H

#include "../include/utils.h"

/* MACROS */
#define ARG_CLT_TOTAL       6
#define ARG_SRV_TOTAL       8
#define ARG_PROG            1
#define ARG_MY_IP           2
#define ARG_HOST_IP         3
#define ARG_MIN_DELAY       4
#define ARG_MAX_DELAY       5
#define ARG_MASK            6
#define ARG_DEVICE          7
#define CLIENT              0
#define SERVER              1

/* FUNCTION PROTOTYPES */
int validate_initial_args(int argc);
int check_for_program_specifier(char **argv);
int valid_clt_args(int argc);
int valid_srv_args(int argc);

void print_clt_usage();
void print_srv_usage();
void print_usage();
void extract_clt_args(char **argv, Client_Args *clt_args);
void extract_srv_args(char **argv, Server_Args *srv_args);

#endif
