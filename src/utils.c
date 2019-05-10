/*==============================================================================
|   SOURCE:     utils.c
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Source file that contains functions calls that are shared
|               amongst client and server.
==============================================================================*/
#include "../include/utils.h"
#include <stdio.h>
#include <sys/socket.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

/*------------------------------------------------------------------------------
|   FUNCTION:   int create_socket(int domain, int type, int protocol)
|					domain : communication domain
|					type : communication semantics
|					protocol : protocol to use
|
|   RETURN:     -1 on error, greater than 0 on success
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Wrapper functionthat creates a socket based on arguments
|	            provided.
------------------------------------------------------------------------------*/
int create_socket(int domain, int type, int protocol)
{
    int _sd;
    if((_sd = socket(domain, type, protocol)) == -1)
    {
        printf("\nError creating socket");
        printf("\nERR CODE: %s", strerror(errno));
    }
    return _sd;
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void forge_ip(Packet *pkt, __u8 type, __u32 src, __u32 dst)
|                   *pkt - packet to fill out
|                   type - type of packet being sent (data encoded in IPs TOS field)
|                   src - IPs source field
|                   dst - IPs destination field
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Fills out IP header for packet.
------------------------------------------------------------------------------*/
void forge_ip(Packet *pkt, __u8 type, __u32 src, __u32 dst)
{
    pkt->ip.ihl = 5;
    pkt->ip.version = 4;
    pkt->ip.tos = type;
    pkt->ip.tot_len = htons(sizeof(pkt));
    pkt->ip.id = 0;
    pkt->ip.frag_off = 0;
    pkt->ip.ttl = htons(256);
    pkt->ip.protocol = IPPROTO_TCP;
    pkt->ip.saddr = src;
    pkt->ip.daddr = dst;
}

/*------------------------------------------------------------------------------
|   FUNCTION:   void forge_tcp(Packet *pkt)
|                   *pkt - packet to fill out
|
|   RETURN:     void
|
|   AUTHOR:     Alex Zielinski
|
|   DATE:       Dec 3, 2018
|
|   INFO:       Fills out TCP header for packet.
------------------------------------------------------------------------------*/
void forge_tcp(Packet *pkt)
{
    pkt->tcp.source = htons(rand() % 15000 + 7000);
    pkt->tcp.dest = htons(rand() % 15000 + 7000);
    pkt->tcp.seq = 1+(int)(10000000.0*rand()/(RAND_MAX+1.0));
    pkt->tcp.ack_seq = 0;
    pkt->tcp.res1 = 0;
    pkt->tcp.doff = 5;
    pkt->tcp.fin = 0;
    pkt->tcp.syn = 1;
    pkt->tcp.rst = 0;
    pkt->tcp.psh = 0;
    pkt->tcp.ack = 0;
    pkt->tcp.urg = 0;
    pkt->tcp.window = 512;
    pkt->tcp.urg_ptr = 0;
}

/*------------------------------------------------------------------------------
|   FUNCTION:   unsigned short in_cksum(unsigned short *ptr, int nbytes)
|
|   DATE:       Dec 3, 2018
|
|   INFO:
|       clipped from ping.c
|       Copyright (c)1987 Regents of the University of California
|       All rights reserved.
|
|           Redistribution and use in source and binary forms are permitted
|       provided that the above copyright notice and this paragraph are
|       dupliated in all such forms and that any documentation, advertising
|       materials, and other materials related to such distribution and use
|       acknowledge that the software was developed by the University of
|       California, Berkeley. The name of the University may not be used
|       to endorse or promote products derived from this software without
|       specific prior written permission. THIS SOFTWARE IS PROVIDED ``AS
|       IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
|       WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHATIBILITY AND
|       FITNESS FOR A PARTICULAR PURPOSE
------------------------------------------------------------------------------*/
unsigned short in_cksum(unsigned short *ptr, int nbytes)
{
    register long		sum;		/* assumes long == 32 bits */
    u_short			oddbyte;
    register u_short	answer;		/* assumes u_short == 16 bits */

    /*
     * Our algorithm is simple, using a 32-bit accumulator (sum),
     * we add sequential 16-bit words to it, and at the end, fold back
     * all the carry bits from the top 16 bits into the lower 16 bits.
     */

    sum = 0;
    while (nbytes > 1)  {
    	sum += *ptr++;
    	nbytes -= 2;
    }

    /* mop up an odd byte, if necessary */
    if (nbytes == 1) {
    	oddbyte = 0;		/* make sure top half is zero */
    	*((u_char *) &oddbyte) = *(u_char *)ptr;   /* one byte only */
    	sum += oddbyte;
    }

    /*
     * Add back carry outs from top 16 bits to low 16 bits.
     */

    sum  = (sum >> 16) + (sum & 0xffff);	/* add high-16 to low-16 */
    sum += (sum >> 16);			/* add carry */
    answer = ~sum;		/* ones-complement, then truncate to 16 bits */
    return(answer);
}
