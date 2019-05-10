// server.h
#ifndef SERVER_H
#define SERVER_H

#include "../include/utils.h"
#include <pcap/pcap.h>

/* FUNCTION PROTOTYPES */
char clt_cmd[BUF_MED];
int send_file(char *filename, char type, Server_Args *srv_args);

void run_server(Server_Args srv_args);
void run_read_loop(Server_Args srv_args);
void process_clt_cmd(Server_Args *srv_args);
void execute_cmd();
void pcap_srv_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

#endif
