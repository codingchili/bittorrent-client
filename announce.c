/* announce.c 
 * 2014-04-16
 * Robin Duda
 * Reference URL: http://beej.us/guide/bgnet/output/html/multipage/syscalls.html
 * Announces presence to tracker and fetches peers in the swarm.
 */

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 
#include "urlparse.h"

 //todo: save a peerlist with ip:port and info_hash.


int build(char request[200], char* tracker, char* info_hash, char* peer_id, char* ip, 
              char* event, int downloaded, int left) 
{
    char* announce = (char*) malloc(strlen(tracker));
    char* hostname = (char*) malloc(strlen(tracker));
    int port = 80;

    url_port(tracker, &port);
    url_hostname(tracker, hostname);
    url_announce(tracker, announce);

    strcat(request, "GET ");
    strcat(request, announce);
    strcat(request, "?info_hash=");
    strcat(request, info_hash);
    strcat(request, "&peer_id=");
    strcat(request, peer_id);
    strcat(request, "&port=");
    strcat(request, "31337");
    strcat(request, "&downloaded=");
    strcat(request, "0");
    strcat(request, "&left=");
    strcat(request, "12852");
    strcat(request, "&event=");
    strcat(request, event);
    strcat(request, " HTTP/1.1\r\n");
    strcat(request, "host: ");
    strcat(request, hostname);
    strcat(request, "\r\n\r\n");

    return strlen(request);
}

void query(char request[200], char* tracker, int* sockfd)
{
    int n = 0, port = 80, url_len = strlen(tracker);
    char* hostname = (char*) malloc(url_len);
    char* protocol = (char*) malloc(url_len);
    char recvbuf[1024];
    struct addrinfo hints, *res;

    url_hostname(tracker, hostname);
    url_protocol(tracker, protocol);
    url_port(tracker, &port);

    //printf("\n\nannounce request!\nurl: %s\nhostname: %s\nannounce: %s\nprotocol: %s\nport: %d\n\n", tracker, hostname, announce, protocol, port);
    memset(&hints, 0, sizeof(hints));
    memset(recvbuf, '0',sizeof(recvbuf));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    getaddrinfo(hostname, protocol, &hints, &res);

    if ((*sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
    {
        printf("\n Error : Could not create socket \n");
        return;
    } 

    if (connect(*sockfd, res->ai_addr, res->ai_addrlen) < 0)
    {
       printf("\n Error : Connect Failed \n");
       return;
    } 

    send(*sockfd, request, strlen(request), 0);
}

void response(int* sockfd)
{
    int num, msglen = 1;
    char recvbuf[1024];
 
    memset(recvbuf, '0', sizeof(recvbuf));

    while ((num = read(*sockfd, recvbuf, msglen)) > 0) //sizeof(recvbuf)-1) = nbytes (1)
    {
        recvbuf[num] = 0;
        if(fputs(recvbuf, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
            return;
        }

        if (strcmp(recvbuf, ":"))
            msglen = 5;
        else
            msglen = 1;

        int dls = 0;
        char terminate = '0';
        if (strcmp(recvbuf, "peers") == 0)
        {

            while (terminate != ':')    //skip length, skip : char
             read(*sockfd, &terminate, 1);


while (num > 0)
{
            int sx = 0;

            printf("\n\nIP: ");
            for (sx = 0; sx < 4; sx++)
            {
                num = read(*sockfd, &dls, 1);
                printf("%d.", dls);
            }

            //x * 2^⁸ -1
            int port = 0;
            dls = 0;
            num = read(*sockfd, &dls, 1);
            printf(" %d ", dls);

            //swap byte order
            if (dls != 0)
                port = (dls * 256) ;

            dls = 0;
            num = read(*sockfd, &dls, 1);
            printf(" %d ", dls);
            printf("\n");
            port += dls;

            printf("\nPORT: %d\n", port);
        }
        }

    } 

    if (num < 0)
    {
        printf("\n Read error \n");
        return;
    }  

}

//ip is set to 0 for non-proxy connections.
//TODO: Return list of peers.
int tracker_announce(char* tracker, char* info_hash, char* peer_id, char* ip, 
              char* event, int downloaded, int left) 
{
    int sockfd = 0;
    char request[200];

    build(request, tracker, info_hash, peer_id, ip, event, downloaded, left);   //bound port
    query(request, tracker, &sockfd);                                                           //target port in url
    response(&sockfd);                                                                                 //todo: bind/listen

    return 0;
}