add_custom_command(
  OUTPUT 
  datalocationinterfaceC.cpp
  datalocationinterfaceClient.cpp
  datalocationinterface.h
  COMMAND
  wsdl2h -o ${CMAKE_CURRENT_SOURCE_DIR}/datalocationinterface.h
         -t ${CMAKE_CURRENT_SOURCE_DIR}/catalogs_typemap.dat
         ${CMAKE_CURRENT_SOURCE_DIR}/DataLocationInterface.wsdl
  COMMAND 
  soapcpp2 -I/usr/include:/usr/import:/usr/share/gsoap/import:/usr/include/gsoap -C -n -p datalocationinterface ${CMAKE_CURRENT_SOURCE_DIR}/datalocationinterface.h
  )

add_custom_target(GENSTUBS ALL DEPENDS 
  ${CMAKE_CURRENT_SOURCE_DIR}/datalocationinterfaceC.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/datalocationinterfaceClient.cpp
  )

include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
		    ${PROJECT_SOURCE_DIR}/src
		    ${PROJECT_SOURCE_DIR}/src/helper/broker
                    ${MAIN_PROJECT_SOURCE_DIR}
                    ${MAIN_PROJECT_SOURCE_DIR}/common/interface
		    ${WMS_COMMON_CONF_INCLUDE_DIRS}
		    ${GLOBUS_GSSAPI_INCLUDE_DIRS}
		    /usr/include)

add_custom_target(BROKER
  DEPENDS 
  classad_plugin_loader.cpp
  classad_plugin.cpp
  doMatch.cpp
  listAttrRegEx.cpp
  measured_response_time.cpp
  successFraction.cpp
  testMemberEx.cpp
  retrieveCloseSEsInfo.cpp
  matchmaker.cpp
  matchmakerISMImpl.cpp
  mm_exceptions.cpp
  datalocationinterfaceC.cpp
  datalocationinterfaceClient.cpp
  DataLocationInterfaceSOAP.cpp
  storage_utils.cpp
  dli_utils.cpp
  brokerinfo.cpp
  ResourceBroker.cpp
  RBSimpleISMImpl.cpp
  RBMaximizeFilesISMImpl.cpp
  RBSelectionSchema.cpp
  maxRankSelector.cpp
  stochasticRankSelector.cpp
  exceptions.cpp
  Helper.h
  Helper_ism.cpp
  Helper_matcher_ism.cpp
  )

add_library(glite_wms_helper_broker_ism
  SHARED 
  classad_plugin_loader.cpp
  classad_plugin.cpp
  doMatch.cpp
  listAttrRegEx.cpp
  measured_response_time.cpp
  successFraction.cpp
  testMemberEx.cpp
  retrieveCloseSEsInfo.cpp
  matchmaker.cpp
  matchmakerISMImpl.cpp
  mm_exceptions.cpp
  datalocationinterfaceC.cpp
  datalocationinterfaceClient.cpp
  DataLocationInterfaceSOAP.cpp
  storage_utils.cpp
  dli_utils.cpp
  brokerinfo.cpp
  ResourceBroker.cpp
  RBSimpleISMImpl.cpp
  RBMaximizeFilesISMImpl.cpp
  RBSelectionSchema.cpp
  maxRankSelector.cpp
  stochasticRankSelector.cpp
  exceptions.cpp
  Helper.h
  Helper_ism.cpp
  Helper_matcher_ism.cpp
  )

set_target_properties(glite_wms_helper_broker_ism PROPERTIES VERSION 0.0.0 SOVERSION 0)

target_link_libraries(glite_wms_helper_broker_ism
  ${JDL_API_CPP_LDFLAGS}
  ${WMS_COMMON_CONF_LDFLAGS}
  ${WMS_COMMON_LOGGER_LDFLAGS}
  ${CLASSAD_LDFLAGS}
  ${BOOST_REGEX_LDFLAGS}
  ${BOOST_DATE_TIME_LDFLAGS}
  ${BOOST_FILESYSTEM_LDFLASGS}
  ${BOOST_THREAD_LDFLAGS}
  ${WMSUTILS_CLASSADS_LDFLAGS}
  ${WMSUTILS_JOBMAN_EXCEPTION_LDFLAGS}
  ${LB_CLIENT_LDFLAGS}
  ${GSOAP_PLUGIN_LDFLAGS}
  ${GSOAP_SSL_PLUSPLUS_LDFLAGS}
  )


install(TARGETS glite_wms_helper_broker_ism
  LIBRARY 
  DESTINATION 
  ${PREFIX}/stage/usr/${LIBDIR}
  )
