/*
  Copyright (c) 2004 on behalf of the EU EGEE Project:
  The European Organization for Nuclear Research (CERN),
  Istituto Nazionale di Fisica Nucleare (INFN), Italy
  Datamat Spa, Italy
  Centre National de la Recherche Scientifique (CNRS), France
  CS Systeme d'Information (CSSI), France
  Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
  Universiteit van Amsterdam (UvA), Netherlands
  University of Helsinki (UH.HIP), Finland
  University of Bergen (UiB), Norway
  Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom
  
  Mock LB implementation
  
  Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
  
  Version info: $Id$
  
 */

#include "glite/lb/context.h"
#include "glite/lb/consumer.h"
#include "mockLB.h"

#ifdef __cplusplus
extern "C" {
#endif

char*
edg_wll_GetSequenceCode( const edg_wll_Context	context )
{
  return strdup("00001");
}

int 
edg_wll_LogMatch(edg_wll_Context context, const char * dest_id){
  return 0;
}

int 
edg_wll_LogDeQueued(edg_wll_Context context, const char * queue, const char * local_jobid){
  return 0;
}

int
edg_wll_LogCancelREQ(edg_wll_Context context, const char * reason){
  return 0;
}

int
edg_wll_LogCancelDONE(edg_wll_Context context, const char * reason){
  return 0;
}

int
edg_wll_LogAbort(edg_wll_Context context, const char * reason){
  return 0;
}

int
edg_wll_LogPending(edg_wll_Context context, const char * reason){
  return 0;
}

int 
edg_wll_LogFlush( edg_wll_Context context,struct timeval *timeout){
  return 0;
}

int  
edg_wll_SetParam( edg_wll_Context context, edg_wll_ContextParam param, ...){
  return 0;
}

int
edg_wll_Error(edg_wll_Context context, char **errText, char **eddDesc){
  return 0;
}

int
edg_wll_SetLoggingJob(edg_wll_Context context, const edg_wlc_JobId job,const char *code,int flags){
  return 0;
}

int
edg_wll_QueryEvents(
	edg_wll_Context context, const edg_wll_QueryRec *job_conditions,
	const edg_wll_QueryRec *event_conditions, edg_wll_Event **events){
  return 0;
}

int
edg_wll_GetLoggingJob(const edg_wll_Context context,edg_wlc_JobId *jobid_out){
  return 0;
}

void
edg_wll_FreeEvent(edg_wll_Event *event){
}

int 
edg_wll_InitContext(edg_wll_Context *context){
  return 0;
}

void 
edg_wll_FreeContext(edg_wll_Context context){
}

#ifdef __cplusplus
}
#endif
