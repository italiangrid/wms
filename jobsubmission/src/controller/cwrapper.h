/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
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
