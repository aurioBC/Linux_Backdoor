// client.h
#ifndef CLIENT_H
#define CLIENT_H

#include "../include/utils.h"
#include <pcap/pcap.h>

/* FUNCTION PROTOTYPES */
char dw_filename[BUF_MED];
char *get_cmd(Client_Args clt_args);
int process_cmd(char cmd[BUF_MED], Client_Args clt_args);
int send_cmd(char cmd[BUF_MED], Client_Args clt_args);

void run_client(Client_Args clt_args);
void run_cmd_loop(Client_Args clt_args);
void read_cmd_response(Pcap_Args *pcap_args);
void setup_pcap_loop(Pcap_Args *pcap_args);
void append_data(char *filename, char c);
void get_filename(char *path, char name[BUF_SML]);
void pcap_clt_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void pcap_dw_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
void *dw_sniffer(void *ptr);

#endif
