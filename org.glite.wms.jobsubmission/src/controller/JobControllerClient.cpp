#include "JobControllerFactory.h"
#include "JobControllerClient.h"
#include "JobControllerClientImpl.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

JobControllerClient::JobControllerClient()
  : jcc_impl(JobControllerFactory::instance()->create_client())
{ }

JobControllerClient::~JobControllerClient()
{ delete this->jcc_impl; }

void JobControllerClient::release_request()
{
  this->jcc_impl->release_request();
}

void JobControllerClient::extract_next_request()
{
  this->jcc_impl->extract_next_request();
}

const Request *JobControllerClient::get_current_request()
{
  return this->jcc_impl->get_current_request();
}

std::string const JobControllerClient::get_current_request_name() const
{
  return this->jcc_impl->get_current_request_name();
}

}} JOBCONTROL_NAMESPACE_END;
