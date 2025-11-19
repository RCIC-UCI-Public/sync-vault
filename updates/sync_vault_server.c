#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <rpc/rpc.h>
#include "sync_vault.h"

#define BUFFER_SIZE 1024

static int debug = 0;
static char script_path[BUFFER_SIZE] = "";

void debug_print(const char *msg) {
    if (debug) {
        char timestr[64];
        time_t t = time(NULL);
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&t));
        printf("[%s] %s\n", timestr, msg);
        fflush(stdout);
    }
}

int is_privileged_port(SVCXPRT *transp) {
    struct sockaddr_in *caller = (struct sockaddr_in *)svc_getcaller(transp);
    if (!caller) return 0;
    int port = ntohs(caller->sin_port);
    return (port > 0 && port < 1024);
}

/*
 * The key fix: 'res' is a static pointer to heap-allocated string.
 * Each response allocates a new heap string, which XDR will free.
 */
char **exec_script_1_svc(char **argp, struct svc_req *rqstp) {
    static char *res = NULL;

    // Free any previous result to avoid leaks
    if (res) {
        free(res);
        res = NULL;
    }

    char localbuf[BUFFER_SIZE];
    localbuf[0] = '\0';

    SVCXPRT *transp = rqstp->rq_xprt;
    char dbgmsg[BUFFER_SIZE + 64];

    snprintf(dbgmsg, sizeof(dbgmsg),
             "Received RPC call from %s:%d: '%s'",
             inet_ntoa(((struct sockaddr_in *)svc_getcaller(transp))->sin_addr),
             ntohs(((struct sockaddr_in *)svc_getcaller(transp))->sin_port),
             (*argp) ? *argp : "(null)");
    debug_print(dbgmsg);

    if (is_privileged_port(transp)) {
        snprintf(dbgmsg, sizeof(dbgmsg), "Running script: %s '%s'", script_path, *argp);
        debug_print(dbgmsg);

        pid_t pid = fork();
        if (pid == 0) {
            execl(script_path, script_path, *argp, (char *)NULL);
            perror("execl");
            _exit(127);
        }
        snprintf(localbuf, sizeof(localbuf), "Script executed with argument: %s", *argp);
    } else {
        snprintf(localbuf, sizeof(localbuf), "Refused: client port not privileged");
        debug_print("Refused: client port not privileged");
    }

    // Dynamically allocate the response buffer and copy response
    res = malloc(strlen(localbuf) + 1);
    if (!res) {
        static char *errmsg = "Server malloc failed";
        return &errmsg;
    }
    strcpy(res, localbuf);

    // Always return pointer to the static (persisting) pointer-to-string
    return &res;
}

int main(int argc, char *argv[]) {
    SVCXPRT *transp;
    script_path[0] = '\0';

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            strncpy(script_path, argv[++i], sizeof(script_path) - 1);
            script_path[sizeof(script_path) - 1] = '\0';
        } else if (strcmp(argv[i], "-d") == 0) {
            debug = 1;
        }
    }

    if (script_path[0] == '\0') {
        fprintf(stderr, "Usage: %s -s <script> [-d]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pmap_unset(SYNC_VAULT_PROG, SYNC_VAULT_VERS);

    transp = svcudp_create(RPC_ANYSOCK);
    if (!transp) {
        fprintf(stderr, "Cannot create UDP service.\n");
        exit(EXIT_FAILURE);
    }

    extern void sync_vault_prog_1(struct svc_req *, SVCXPRT *);
    if (!svc_register(transp, SYNC_VAULT_PROG, SYNC_VAULT_VERS,
                      sync_vault_prog_1, IPPROTO_UDP)) {
        fprintf(stderr,
                "Unable to register (SYNC_VAULT_PROG, SYNC_VAULT_VERS, UDP)\n");
        exit(EXIT_FAILURE);
    }

    debug_print("RPC Server registered and listening...");
    svc_run();

    fprintf(stderr, "svc_run returned unexpectedly\n");
    return 1;
}
