/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://project.eu-egee.org/index.php?id=115 for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#ifndef GLITE_WMS_COMMON_CONFIGURATION_INITIALIZER_H
#define GLITE_WMS_COMMON_CONFIGURATION_INITIALIZER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { edg_wlcc_OK,
	       edg_wlcc_error,
	       edg_wlcc_exists,
	       edg_wlcc_uninitialized,
	       edg_wlcc_mustrealloc,
} edg_wlcc_configuration_Error;
typedef edg_wlcc_configuration_Error config_error_t;

config_error_t edg_wlcc_ConfigurationInitialize( const char *filename, const char *module );
void edg_wlcc_ConfigurationFree( void );
  void edg_wlcc_SetConfigurationError( const char *error );
const char *edg_wlcc_ConfigurationError( void );

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* GLITE_WMS_COMMON_CONFIGURATION_INITIALIZER_H */

// Local Variables:
// mode: c
// End:
