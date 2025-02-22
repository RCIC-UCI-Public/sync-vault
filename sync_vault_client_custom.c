/*
 * This is sample code generated by rpcgen.
 * These are only templates and you can use them
 * as a guideline for developing your own functions.
 */

/* This is customized to attempt to use a privileged src port */
#include "sync_vault.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

char* get_ip_address(const char *hostname) {
    struct hostent *he;
    struct in_addr **addr_list;
    char *ip = (char *) malloc((INET6_ADDRSTRLEN + 1) * sizeof(char));
    
    if ((he = gethostbyname(hostname)) == NULL) {
        perror("gethostbyname");
        return NULL;
    }
    
    addr_list = (struct in_addr **)he->h_addr_list;
    
    if (addr_list[0] != NULL) {
        strcpy(ip, inet_ntoa(*addr_list[0]));
        printf ("got ip address: %s\n",ip);
        return ip;
    }
    
    return (char *) NULL;
}

void
sync_vault_1(char *host)
{
	CLIENT *clnt;
	int  *result_1;
	int sync_vault_ping_1_arg1;
        int port_low = 100;
        int port_high = 1024;
        int sock;
        struct sockaddr_in sin;
        struct sockaddr_in lin;
        char * serverip;
        int OK;
        struct timeval DEFAULT_TIMEOUT;
        DEFAULT_TIMEOUT.tv_sec = 10;  // 1 seconds
        DEFAULT_TIMEOUT.tv_usec = 0; // 0 microseconds

        /* --- Open a socket and bind to a port in the specific port range --- */
        if ( (serverip = get_ip_address(host)) == (char *) NULL )
        {
            fprintf(stderr, "could not resolve %s\n", host);
            exit(1);
        }

        //  Zero the socket struct memory then configure to talk to the remote

        memset(&sin, 0, sizeof(sin));
        sin.sin_family = AF_INET;
        sin.sin_addr.s_addr = inet_addr(serverip);
        sin.sin_port = htons(0); // Let the system choose a port

        // Create a socket 
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        if (sock < 0) {
            perror("socket");
            exit(1);
        }

        // Try to bind to a port in the specified range
        for (int port = port_low; port <= port_high; port++) {
             printf("trying port %d \n", port);
             lin.sin_family = AF_INET;
             lin.sin_addr.s_addr = INADDR_ANY;
             lin.sin_port = htons(port);

             if ( (OK = bind(sock, (struct sockaddr *)&lin, sizeof(lin))) == 0) {
                   break; // Successful binding
             } else if (errno != EADDRINUSE) {
                   perror("bind");
                   exit(1);
             }
        }
        if (OK != 0 ) {
            fprintf(stderr, "No available sockets in range [%d-%d]\n", port_low, port_high);
            exit(1);
        }


        // Create the RPC client
        printf("Creating RPC Clinet \n");
        clnt = clntudp_create(&sin, SYNC_VAULT, SYNC_VAULT_VERS, DEFAULT_TIMEOUT,&sock);
        if (clnt == NULL) {
            clnt_pcreateerror("clntudp_create");
            exit(1);
        }
        printf("Calling sync rpc\n");
	result_1 = sync_vault_ping_1(sync_vault_ping_1_arg1, clnt);
	if (result_1 == (int *) NULL) {
		clnt_perror (clnt, "call failed");
	}
       
        /* -- Clean up Memory -- */
	clnt_destroy (clnt);
        close(sock);
        free(serverip); 
}


int
main (int argc, char *argv[])
{
	char *host;

	if (argc < 2) {
		printf ("usage: %s server_host\n", argv[0]);
		exit (1);
	}
	host = argv[1];
	sync_vault_1 (host);
        exit (0);
}
