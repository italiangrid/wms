// File: successFraction.cpp
// Copyright (c) 2004 on behalf of the EU EGEE Project:
// The European Organization for Nuclear Research (CERN),
// Istituto Nazionale di Fisica Nucleare (INFN), Italy
// Datamat Spa, Italy
// Centre National de la Recherche Scientifique (CNRS), France
// CS Systeme d'Information (CSSI), France
// Royal Institute of Technology, Center for Parallel Computers (KTH-PDC), Sweden
// Universiteit van Amsterdam (UvA), Netherlands
// University of Helsinki (UH.HIP), Finland
// University of Bergen (UiB), Norway
// Council for the Central Laboratory of the Research Councils (CCLRC), United Kingdom

// $Id$

#include <classad_distribution.h>
#include <fnCall.h>
#include <boost/scoped_ptr.hpp>

#include "glite/lb/statistics.h"
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"

namespace configuration = glite::wms::common::configuration;

using namespace std;
#ifdef WANT_NAMESPACES
using namespace classad;
#endif

// How far to the history we look
// XXX: should not be hardcoded
#define	INTERVAL	300

namespace glite {
namespace wms {
namespace classad_plugin {
namespace lb_rank {

namespace {
	edg_wll_Context      lb_rank_log_ctx = NULL; 
	std::string          host_proxy_config;
	std::string          lb_server;
	std::string          lb_server_config;
	int                  lb_port;
	pthread_mutex_t	     lb_rank_ctx_mutex = PTHREAD_MUTEX_INITIALIZER;
} // End of anonymous namespace

bool successFraction(const char         *name,
		const ArgumentList &arguments,
		EvalState          &state,
		Value              &result)
{
	bool  eval_successful = false;
	result.SetErrorValue();
	// We check to make sure that we are passed at least one argument
	if (arguments.size() >= 1) {
		
		// ...the first argument (CEid) should evaluate to a string...
		// the first arguments (CEid) should evaluate to a string...
		Value       arg1;
		std::string ceid;
		if (arguments[0] -> Evaluate(state, arg1) &&
				arg1.IsStringValue(ceid)) {

			edg_wll_QueryRec	group[2];
			time_t	now,before;
			int	resf,rest;
			float	ok,fail;
			double computed_fraction = 1.0; // if anything goes wrong

			ok = fail = 0.0;
			eval_successful = true;

			// Set context if necessary and do whatever else
			// is needed...

			group[0].attr = EDG_WLL_QUERY_ATTR_DESTINATION;
			group[0].op = EDG_WLL_QUERY_OP_EQUAL;
			// discarding const is OK here
			group[0].value.c = (char *) ceid.c_str();
			group[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;

			time(&now);
			before = now - INTERVAL;

			// Figure out the LB server name and port
			// from 2nd argument or local config.
		        std::string lb_server_port;
			if (arguments.size() >= 2) {
				Value arg2;
				if (arguments[1] -> Evaluate(state, arg2) ) {
					arg2.IsStringValue(lb_server_port); 
				}
			}

			// Find the host proxy from config.
			if (host_proxy_config.empty()) {
				configuration::Configuration const* const config = configuration::Configuration::instance();			
				if (config) {
					configuration::CommonConfiguration const* const common_config = config->common();

					host_proxy_config = common_config->host_proxy_file();
				}
			}

			if (lb_server_port.empty() && lb_server_config.empty()) {
				// Fetch LB server from config and cache
				// it.
				configuration::Configuration const* const config = configuration::Configuration::instance();			
				if (config) {
					configuration::WMPConfiguration const* const wp_config = config->wp();
					lb_server_config = wp_config->lbserver().front();
				}
			}
			if (lb_server_port.empty() && !(lb_server_config.empty())) {
				// Use LB server from config
				lb_server_port = lb_server_config;
			}
			std::string this_lb_server;
			int         this_lb_port;
			if (!(lb_server_port.empty())) {
				std::string::size_type colon;
				if ((colon = lb_server_port.find(':')) != std::string::npos) {
					this_lb_server = lb_server_port.substr(0,colon);
					this_lb_port = std::atoi(lb_server_port.substr(colon+1).c_str());
				} else {
					this_lb_server = lb_server_port;
					this_lb_port = 9000;
				}
			}
			// paranoia: can this lock ever fail?
			if (!pthread_mutex_lock(&lb_rank_ctx_mutex)) { 
				// Set up LB server and port
				if ( !lb_rank_log_ctx ||
				     (this_lb_server != lb_server) ||
				     (this_lb_port != lb_port) ) {
				        if (lb_rank_log_ctx) edg_wll_FreeContext(lb_rank_log_ctx);
				        edg_wll_InitContext(&lb_rank_log_ctx);

					edg_wll_SetParam(lb_rank_log_ctx, EDG_WLL_PARAM_QUERY_SERVER_PORT, this_lb_port);
					edg_wll_SetParam(lb_rank_log_ctx, EDG_WLL_PARAM_QUERY_SERVER, this_lb_server.c_str());
					edg_wll_SetParam(lb_rank_log_ctx, EDG_WLL_PARAM_X509_PROXY, host_proxy_config.c_str() );

					lb_server = this_lb_server;
					lb_port = this_lb_port;
				}

				// Compute what needs to be computed
				if (edg_wll_StateRate(lb_rank_log_ctx,group,
					EDG_WLL_JOB_DONE,EDG_WLL_STAT_FAILED,
					&before,&now,&fail,&resf,&rest) ||
				    edg_wll_StateRate(lb_rank_log_ctx,group,
					EDG_WLL_JOB_DONE,EDG_WLL_STAT_OK,
					&before,&now,&ok,&resf,&rest))
				{
					// XXX: failure, we should complain
					//	instead, we are tooooooo optimistic
					computed_fraction = 1.0;
				}
				// XXX: again, is it ok to report 1.0 if we know 
				// 	nothing?
				else computed_fraction = ok + fail == 0.0 ?
						1.0 : ok / (ok + fail);
	
				pthread_mutex_unlock(&lb_rank_ctx_mutex);
			}
				
			// Return result
			result.SetRealValue(computed_fraction);
		}
	}
	
	return eval_successful;
}

} // namespace gangmatch
} // namespace clasad_plugin
} // namespace wms
} // namespace glite
