#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <rpc/rpc.h>
#include "sync_vault.h"

#define BUFFER_SIZE 1024

void debug_print(const char *msg, int debug) {
    if (debug) {
        char timestr[64];
        time_t t = time(NULL);
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&t));
        printf("[%s] %s\n", timestr, msg);
    }
}

int main(int argc, char *argv[]) {
    int debug = 0;
    const char *server_host = NULL;
    int argi = 1;

    if (argc > 1 && strcmp(argv[argi], "-d") == 0) {
        debug = 1;
        argi++;
    }
    if (argc <= argi) {
        fprintf(stderr, "Usage: %s [-d] <server_host|IP>\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    server_host = argv[argi];

    char local_hostname[BUFFER_SIZE];
    if (gethostname(local_hostname, sizeof(local_hostname)) != 0) {
        perror("gethostname");
        exit(EXIT_FAILURE);
    }
    local_hostname[BUFFER_SIZE-1] = '\0';

    CLIENT *clnt;
    clnt = clnt_create(server_host, SYNC_VAULT_PROG, SYNC_VAULT_VERS, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(server_host);
        exit(EXIT_FAILURE);
    }

    debug_print("Calling remote EXEC_SCRIPT RPC", debug);
    char *argp = local_hostname;
    char *result = exec_script_1(&argp, clnt);
    if (!result) {
        clnt_perror(clnt, "call failed:");
    } else {
        printf("Server Response: %s\n", result);
    }

    clnt_destroy(clnt);
    return 0;
}
