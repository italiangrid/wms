#include <string>
#include <iostream>
#include <cstdlib>
#include <cstring>

#include "glite/lb/statistics.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/configuration/WMPConfiguration.h"

int main(int argc,char **argv)
{
	glite::wms::common::configuration::Configuration c(
		"glite_wms.conf",
		glite::wms::common::configuration::ModuleType::workload_manager
	);
	std::string host_proxy = c.common()->host_proxy_file();
	std::string lb_server = c.wp()->lbserver().front();

	std::cout << "LB server: " << lb_server << '\n';
	std::cout << "Host proxy: " << host_proxy << '\n';

        edg_wll_Context ctx;
        edg_wll_InitContext(&ctx);

        edg_wll_SetParamInt(ctx, EDG_WLL_PARAM_QUERY_SERVER_PORT, 9000);
        edg_wll_SetParamString(ctx, EDG_WLL_PARAM_QUERY_SERVER, lb_server.c_str());
	edg_wll_SetParamString(ctx, EDG_WLL_PARAM_X509_PROXY, host_proxy.c_str());

        // the only supported grouping for now
        edg_wll_QueryRec group[2];
        group[0].attr = EDG_WLL_QUERY_ATTR_DESTINATION;
        group[0].op = EDG_WLL_QUERY_OP_EQUAL;
        if (::strcmp(argv[1], "ALL")) {
                group[0].value.c = "ALL";
        } else {
                group[0].value.c = NULL;
        }
        group[1].attr = EDG_WLL_QUERY_ATTR_UNDEF;


        time_t  from, to, now;
        char    *cfrom,*cto;
        int     from_res,to_res;
        float   *durations, *dispersions;
        char    **groups;
        int     span;
        if (::strcmp(argv[2], "")) {
        	span = 86400;
        } else {
        	span = ::atoi(argv[2]);
	}

        now = to = time(0);
        from = now - span;

        if (
          edg_wll_StateDurationsFromTo(
            ctx,
            group,
            EDG_WLL_JOB_SCHEDULED,
            EDG_WLL_JOB_RUNNING,
            0,
            &from,
            &to,
            &durations,
            &dispersions,
            &groups,
            &from_res,
            &to_res)
        ) {
                char *et,*ed;
                edg_wll_Error(ctx,&et,&ed);
		std::cerr <<  "edg_wll_StateDurationsFromTo(): " << et << ", " << ed << '\n';
                return 1;
        }

        cfrom = ::strdup(ctime(&from));
        cto = ::strdup(ctime(&to));
        cfrom[::strlen(cfrom)-1] = 0;
        cto[::strlen(cto)-1] = 0;

        for (int i = 0; groups[i]; ++i) {
                std::cout << "Average duration at " << groups[i] << ": " << durations[i] << '\n' <<
			"Dispersion index: " << dispersions[i] << '\n' <<
			"  Measuered from " << cfrom << " to " << cto << '\n' <<
			"  With resolution from " << from_res << " to " << to_res << '\n';
	}

        ::free(durations);
        free(dispersions);
        for (int i = 0; groups[i]; ++i)
                free(groups[i]);
        free(groups);
        edg_wll_FreeContext(ctx);

        return 0;
}
