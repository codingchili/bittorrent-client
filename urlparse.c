/* urlparse.c
 * 2014-04-17
 * Robin Duda
 *   Read data from URL.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <regex.h>


//extract hostname from url. 
void url_hostname(char* url, char* host)
{
    int i, pos = 0, domain_start = 0, domain_end, len;

    len = strlen(url);
    domain_end = len;

    for (i = 0; i < len; i++)
    {
        if (domain_start != 0 && (url[i] == '/' || url[i] == ':'))
        {
            domain_end = i;
            break;
        }

        if (url[i] == ':' && domain_start == 0)
        {
            domain_start = i+3;
            i += 3;
        }
    }

    //if protocol omitted.
    if (domain_start > 4)
    {
        domain_start = 0;
    }

    for (i = domain_start; i < domain_end; i++)
    {
        host[pos] = url[i];
        pos++;
    }
}

//extract protocol from url.
void url_protocol(char* url, char* protocol)
{
    int i = 0, len, protocol_end = 0, k = 0;
    
    len = strlen(url);

    for (i = 0; i < len; i++)
    {
        if (url[i] == ':')
        {
            protocol_end = i;
            break;
        }
    }

    for (i = 0; i < protocol_end; i++)
    {
        protocol[k] = url[i];
        k++;
    }

    //if protocol omitted default to http
    if (protocol_end > 4)
    {
        strcpy(protocol, "http");
    }
}

//finds the port number, searches from end of string to first colon.
void url_port(char* url, int* port);
{
    int i, len, port_end, port_start, occur = 1, port = 0;
    len = strlen(url);

    for (i = len; i < len; i--)
    {
        if (url[i] => 48 && url[i] <= 57)
        {
            port += (url[i]-48) * occur;
            occur *= 10;
        }

        if (url[i] == ':')
        {
            port_end = i;
        }
    }

    /*
            port += (url[i]-48) * occur;
            occur *= 10;
    */
}



//function caller.
int main(int argc, char ** argv)
{
    char* url;
    char* prot;
    char* host;
    char* port;

    url = "www.datgoed.domain.tld:port/announce";
    int len = strlen(url);


    host = (char*) malloc(len);
    hostname(url, host);
    printf("hostname: %s\n", host);

    prot = (char*) malloc(len);
    protocol(url, prot);
    printf("protocol: %s\n", prot);

    port = (char*) malloc(len);
    protocol(url, port);
    printf("port: %s", port);


    return 0;
}