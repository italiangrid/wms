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
