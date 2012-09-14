#ifndef EDG_WORKLOAD_JOBCONTROL_CONTROLLER_CWRAPPER_H
#define EDG_WORKLOAD_JOBCONTROL_CONTROLLER_CWRAPPER_H




#ifdef __cplusplus
extern "C" {
#endif

/*
  FWD declaration of opaque structure
*/
struct edg_wljc_jobcontroller_t;

typedef  struct edg_wljc_jobcontroller_t    *edg_wljc_Context;

edg_wljc_Context edg_wljc_ContextInitialize( edg_wll_Context *cont );
void edg_wljc_ContextFree( edg_wljc_Context controller );
void edg_wljc_GetError( edg_wljc_Context controller, char **error );
void edg_wljc_ClearError( edg_wljc_Context controller );
int edg_wljc_IsGood( edg_wljc_Context controller );

int edg_wljc_Submit( edg_wljc_Context controller, const char *ad );
int edg_wljc_Cancel( edg_wljc_Context controller, edg_wlc_JobId id );

#ifdef __cplusplus
}
#endif

#endif /* EDG_WORKLOAD_JOBCONTROL_CONTROLLER_CWRAPPER_H */
