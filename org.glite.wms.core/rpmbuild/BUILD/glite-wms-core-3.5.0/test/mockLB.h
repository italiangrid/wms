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
  
  Mock LB  header file
  
  Authors: Paolo Andreetto <paolo.andreetto@pd.infn.it>
  
  Version info: $Id$
  
 */

#ifndef __GLITE_WMS_MANAGER_MOCK_LB_H__
#define __GLITE_WMS_MANAGER_MOCK_LB_H__

#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _edg_wll_SeqCode {
	unsigned int	c[EDG_WLL_SOURCE__LAST];
} edg_wll_SeqCode;


struct _edg_wll_Context {
  edg_wll_SeqCode	p_seqcode;
};








#ifdef __cplusplus
}
#endif
#endif /* __GLITE_WMS_MANAGER_MOCK_LB_H__ */
