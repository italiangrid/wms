#ident "$Header$"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include <netdb.h>
#include <sys/time.h>
#include <unistd.h>

#include "glite/wmsutils/jobid/cjobid.h"
#include "strmd5.h"

struct _edg_wlc_JobId {
    char		*id;	/* unique job identification */
    /* additional information */
    char		*BShost;/* bookkeeping server hostname */
    unsigned int	BSport;	/* bookkeeping server port */
    char		*info;	/* additional information (after ? in URI) */
};

int edg_wlc_JobIdCreate(const char *bkserver, int port, edg_wlc_JobId *jobId)
{
    return edg_wlc_JobIdRecreate(bkserver, port, NULL, jobId);
}


int edg_wlc_JobIdRecreate(const char* bkserver, int port, const char *unique, edg_wlc_JobId *jobId)
{
    edg_wlc_JobId out;
    char hostname[200]; /* used to hold string for encrypt */
    struct timeval tv;
    int skip;
    char* portbeg;

    struct hostent* he;

    if (!bkserver)
        return EINVAL;

    if (unique == NULL) {
	gethostname(hostname, 100);
	he = gethostbyname(hostname);
	assert(he->h_length > 0);
	gettimeofday(&tv, NULL);
	srandom(tv.tv_usec);

    	skip = strlen(hostname);
    	skip += sprintf(hostname + skip, "-IP:0x%x-pid:%d-rnd:%d-time:%d:%d",
		    *((int*)he->h_addr_list[0]), getpid(), (int)random(),
		    (int)tv.tv_sec, (int)tv.tv_usec);
    }

    *jobId = NULL;
    out = (edg_wlc_JobId) malloc (sizeof(*out));
    if (!out)
	return ENOMEM;

    memset(out, 0, sizeof(*out));

    /* check if it begins with prefix */
    /* unsupported */
    if (strncmp(bkserver, GLITE_WMSC_JOBID_PROTO_PREFIX, sizeof(GLITE_WMSC_JOBID_PROTO_PREFIX)-1) == 0)
        return EINVAL;

    out->BShost = strdup(bkserver);
    portbeg = strchr(out->BShost, ':');
    if (portbeg) {
	*portbeg = 0;
        /* try to get port number */
	if (port == 0)
	    port = atoi(portbeg + 1);
    }

    if (port == 0)
        port = GLITE_WMSC_JOBID_DEFAULT_PORT;

    out->BSport = port;

    out->id = (unique) ? strdup(unique) : str2md5base64(hostname);
    //printf("Encrypt: %s\nBASE64 %s\n", hostname, out->id);

    if (!out->id || !out->BShost) {
	edg_wlc_JobIdFree(out);
	return ENOMEM;
    }

    *jobId = out;
    return 0;
}


int edg_wlc_JobIdDup(const edg_wlc_JobId in, edg_wlc_JobId *out)
{
    edg_wlc_JobId jid;
    *out = NULL;
    if (in == NULL)
	return 0;

    jid = malloc(sizeof(*jid));
    if (!jid)
	return ENOMEM;

    memset(jid, 0,sizeof(*jid));
    jid->BShost = strdup(in->BShost);
    jid->id = strdup(in->id);
    if (in->info)
	jid->info = strdup(in->info);

    if (jid->BShost == NULL || jid->id == NULL) {
	edg_wlc_JobIdFree(jid);
	return ENOMEM;
    }

    jid->BSport = in->BSport;
    *out = jid;
    return 0;
}


// XXX
// use recreate
// parse name, port, unique
int edg_wlc_JobIdParse(const char *idString, edg_wlc_JobId *jobId)
{
    char *pom, *pom1, *pom2;
    edg_wlc_JobId out;

    *jobId = NULL;

    out = (edg_wlc_JobId) malloc (sizeof(*out));
    if (out == NULL )
	return ENOMEM;

    memset(out,0,sizeof(*out));

    if (strncmp(idString, GLITE_WMSC_JOBID_PROTO_PREFIX, sizeof(GLITE_WMSC_JOBID_PROTO_PREFIX) - 1)) {
	out->BShost  = (char *) NULL;
	out->BSport  = 0;

	free(out);
	return EINVAL;
    }

    pom = strdup(idString + sizeof(GLITE_WMSC_JOBID_PROTO_PREFIX) - 1);
    pom1 = strchr(pom, '/');
    pom2 = strchr(pom, ':');

    if (!pom1) { free(pom); free(out); return EINVAL; }

    if ( pom2 && (pom1 > pom2)) {
	pom[pom2-pom]     = '\0';
	out->BShost  = strdup(pom);
	pom[pom1-pom]     = '\0';
	out->BSport  = (unsigned int) strtoul(pom2 + 1,NULL,10);
    } else {
	pom[pom1-pom]     = '\0';
	out->BShost  = strdup(pom);
	out->BSport  = GLITE_WMSC_JOBID_DEFAULT_PORT;
    }

    /* XXX: localhost not supported in jobid 
    if (!strncmp(out->BShost,"localhost",9) {
	free(pom);
	free(out->BShost);
	free(out);
	return EINVAL;
    }
    */

    /* additional info from URI */
    pom2 = strchr(pom1+1,'?');
    if (pom2) {
	*pom2 = 0;
	out->info = strdup(pom2+1);
    }

    /* extract the unique part */
    out->id = strdup(pom1+1);

    for (pom1 = out->BShost; *pom1; pom1++)
	if (isspace(*pom1)) break;

    for (pom2 = out->id; *pom2; pom2++)
	if (isspace(*pom2)) break;

    if (*pom1 || *pom2) {
	    free(pom);
	    edg_wlc_JobIdFree(out);
	    return EINVAL;
    }

    free(pom);
    *jobId = out;
    return 0;
}


void edg_wlc_JobIdFree(edg_wlc_JobId job)
{
    if (job) {
	free(job->id);
	free(job->BShost);
	free(job->info);
	free(job);
    }
}


char* edg_wlc_JobIdUnparse(const edg_wlc_JobId jobid)
{
    char *out, port[40];

    if (!jobid)
	return NULL;

    if (jobid->BSport)
	sprintf(port,":%d",jobid->BSport);
    else
        *port = 0;

    asprintf(&out, GLITE_WMSC_JOBID_PROTO_PREFIX"%s%s/%s%s%s",
	     jobid->BShost,port,
	     jobid->id,
	     (jobid->info ? "?" : ""),
	     (jobid->info ? jobid->info : ""));

    return out;
}


char* edg_wlc_JobIdGetServer(const edg_wlc_JobId jobid)
{
    char *bs = NULL;

    if (jobid)
	asprintf(&bs, "%s:%u", jobid->BShost,
		 jobid->BSport ? jobid->BSport : GLITE_WMSC_JOBID_DEFAULT_PORT);

    return bs;
}


void edg_wlc_JobIdGetServerParts(const edg_wlc_JobId jobid, char **srvName, unsigned int *srvPort)
{
    if (jobid) {
	*srvName = strdup(jobid->BShost);
	*srvPort = jobid->BSport ? jobid->BSport : GLITE_WMSC_JOBID_DEFAULT_PORT;
    }
}


char* edg_wlc_JobIdGetUnique(const edg_wlc_JobId jobid)
{
    return jobid ? strdup(jobid->id) : NULL;
}
