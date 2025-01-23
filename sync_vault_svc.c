/*
 * Please do not edit this file.
 * It was generated using rpcgen.
 */

#include "sync_vault.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpc/pmap_clnt.h>
#include <string.h>
#include <memory.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <syslog.h>
#include <fcntl.h>
#include <syslog.h>
#ifndef SIG_PF
#define SIG_PF void(*)(int)
#endif


void writePidFile(char * pidfile, int pid)
{
        if (pidfile == NULL) return;
        int f;
        char outbuf[128];
        f = open(pidfile,O_CREAT | O_WRONLY);
        if (f <= 0) return;
        sprintf(outbuf,"%d",pid);
        outbuf[sizeof(outbuf)-1] = '\0';
        write(f,outbuf,strlen(outbuf));
        close(f);
}

static int *
_sync_vault_ping_1 (int  *argp, struct svc_req *rqstp)
{
        return (sync_vault_ping_1_svc(*argp, rqstp));
}

static void
sync_vault_1(struct svc_req *rqstp, register SVCXPRT *transp)
{
        union {
        	int sync_vault_ping_1_arg;
        } argument;
        char *result;
        xdrproc_t _xdr_argument, _xdr_result;
        char *(*local)(char *, struct svc_req *);

        switch (rqstp->rq_proc) {
        case NULLPROC:
        	(void) svc_sendreply (transp, (xdrproc_t) xdr_void, (char *)NULL);
        	return;

        case SYNC_VAULT_PING:
        	_xdr_argument = (xdrproc_t) xdr_int;
        	_xdr_result = (xdrproc_t) xdr_int;
        	local = (char *(*)(char *, struct svc_req *)) _sync_vault_ping_1;
        	break;

        default:
        	svcerr_noproc (transp);
        	return;
        }
        memset ((char *)&argument, 0, sizeof (argument));
        if (!svc_getargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
        	svcerr_decode (transp);
        	return;
        }
        result = (*local)((char *)&argument, rqstp);
        if (result != NULL && !svc_sendreply(transp, (xdrproc_t) _xdr_result, result)) {
        	svcerr_systemerr (transp);
        }
        if (!svc_freeargs (transp, (xdrproc_t) _xdr_argument, (caddr_t) &argument)) {
        	fprintf (stderr, "%s", "unable to free arguments");
        	exit (1);
        }
        return;
}

int
main (int argc, char **argv)
{

        char c;
        int debug = 0;
        char * pidfile = "/var/run/sync-vault.pid";
        int pid;

        openlog(SERVICE_NAME, LOG_PID, LOG_LOCAL0);
        syslog(LOG_INFO, "starting service");

        while ((c=getopt(argc,argv,"dp:")) != -1)
        {
        	switch(c)
        	{
        		case 'd': debug = 1;
        			  break;
        		case 'p': pidfile = optarg;
        			  break;
        		default:  break;
        	}
        }
        
            if (! debug) {
            switch ( (pid=fork()) ) {
            case -1:
            	syslog(LOG_ERR, "%s", "cannot fork");
            	exit(1);
            case 0:			/* child process */
            	break;
            default:		/* parent process */
            	if (debug) fprintf(stderr,"pidfile is %s\n", pidfile);
            	writePidFile(pidfile,pid);
            	return 0;
            }
     
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
    
            setsid();        	/* start a new session */
        }

        register SVCXPRT *transp;

        pmap_unset (SYNC_VAULT, SYNC_VAULT_VERS);

        transp = svcudp_create(RPC_ANYSOCK);
        if (transp == NULL) {
        	fprintf (stderr, "%s", "cannot create udp service.");
        	exit(1);
        }
        if (!svc_register(transp, SYNC_VAULT, SYNC_VAULT_VERS, sync_vault_1, IPPROTO_UDP)) {
        	fprintf (stderr, "%s", "unable to register (SYNC_VAULT, SYNC_VAULT_VERS, udp).");
        	exit(1);
        }

        transp = svctcp_create(RPC_ANYSOCK, 0, 0);
        if (transp == NULL) {
        	fprintf (stderr, "%s", "cannot create tcp service.");
        	exit(1);
        }
        if (!svc_register(transp, SYNC_VAULT, SYNC_VAULT_VERS, sync_vault_1, IPPROTO_TCP)) {
        	fprintf (stderr, "%s", "unable to register (SYNC_VAULT, SYNC_VAULT_VERS, tcp).");
        	exit(1);
        }

        svc_run ();
        fprintf (stderr, "%s", "svc_run returned");
        exit (1);
        /* NOTREACHED */
}
