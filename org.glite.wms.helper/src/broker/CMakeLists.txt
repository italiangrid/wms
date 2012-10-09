add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/src/broker/datalocationinterfaceClient.cpp ${PROJECT_SOURCE_DIR}/src/broker/datalocationinterfaceC.cpp DEPENDS ${PROJECT_SOURCE_DIR}/src/broker/datalocationinterface.h COMMAND soapcpp2 -I/usr/include -I/usr/include/gsoap -I/usr/share/gsoap/import -C -n -pdatalocationinterface ${PROJECT_SOURCE_DIR}/src/broker/datalocationinterface.h)

add_custom_command(OUTPUT ${PROJECT_SOURCE_DIR}/datalocationinterface.h DEPENDS ${PROJECT_SOURCE_DIR}/DataLocationInterface.wsdl COMMAND wsdl2h -t ${PROJECT_SOURCE_DIR}/catalogs_typemap.dat -o datalocationinterface.h ${PROJECT_SOURCE_DIR}/DataLocationInterface.wsdl)

add_custom_target(GENSRC ALL DEPENDS ${PROJECT_SOURCE_DIR}/src/broker/datalocationinterfaceC.cpp ${PROJECT_SOURCE_DIR}/src/broker/datalocationinterfaceClient.cpp)

include_directories(${CLASSAD_INCLUDE_DIRS}
                    ${PROJECT_SOURCE_DIR}/interface
                    ${PROJECT_SOURCE_DIR}/src/broker
		    ${CMAKE_INSTALL_PREFIX}/include)

add_custom_target(BROKER DEPENDS 
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
        Helper_matcher_ism.cpp)


add_library(glite_wms_helper_broker_ism SHARED
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
        Helper_matcher_ism.cpp)

set_target_properties(glite_wms_helper_broker_ism PROPERTIES VERSION 0.0.0 SOVERSION 0)

install(TARGETS glite_wms_helper_broker_ism
        LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/${LIBDIR})
