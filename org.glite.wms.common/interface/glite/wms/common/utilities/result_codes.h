#ifndef GLITE_WMS_COMMON_CLIENT_RESULT_CODES_H
#define GLITE_WMS_COMMON_CLIENT_RESULT_CODES_H

namespace glite {
namespace wms {
namespace common {
namespace utilities{
/*
 * result_codes.h
 * Copyright (c) 2001 The European Datagrid Project - IST programme, all rights reserved.
 * Contributors are mentioned in the code where appropriate.
 */


/**
  * Result Code
*/
enum ResultCode {
          SUCCESS,           //  The requested operation has been completed successfully
          ACCEPTED,         // The requested operation has been accepted

          SUBMISSION_FAILURE,  //  API failed, general RB Exc remapping
          CANCEL_FAILURE,    //  API failed, general RB Exc remapping
          GETOUTPUT_FAILURE,    //  API failed, general RB Exc remapping
          STATUS_FAILURE,    //  API failed, general RB Exc remapping

          GETOUTPUT_FORBIDDEN, //When trying to retrieve output from a not submitted job
          CANCEL_FORBIDDEN,  //When trying to cancel a not submitted job
          STATUS_FORBIDDEN,  //When trying to retrieve status from a not submitted job
          ALREADY_SUBMITTED,  //submit skipped because Job has been already submitted

          JOIN_FAILED, //When a pthread_join is waiting for a cored thread

          OUTPUT_NOT_READY,           //JobNotDoneException
          FILE_TRANSFER_ERROR,       //SandboxIOException
          JOB_NOT_FOUND,                  //JobNotFoundException

          MARKED_FOR_REMOVAL, //Cancel Method Result
          GENERIC_FAILURE,             //Cancel Method Result
          CONDOR_FAILURE,             //Cancel Method Result

          GLOBUS_JOBMANAGER_FAILURE,

          JOB_ALREADY_DONE,
          JOB_ABORTED,
          JOB_CANCELLING,
          JOB_NOT_OWNER

};

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif
