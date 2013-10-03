#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <curly/curly_sched.hh>

namespace curly
{
  sched_request::sched_request(reactor::Scheduler& sched,
                               curly::request_configuration conf):
    Operation{sched},
    _request{sched.io_service(), std::move(conf)},
    _code(-1), // -1 is not a bad value...
    _succeed(false)
  {}

  void
  sched_request::raise_if_ecode(CURL* easy,
                                CURLcode code,
                                std::string const& error_message)
  {
    // XXX: duplicated with throw_if_ecode.
    curl_easy_getinfo(easy, CURLINFO_RESPONSE_CODE, &this->_code);

    this->_succeed = ((this->_code == 200) &&
                      (code != CURLE_ABORTED_BY_CALLBACK));

    if (code != CURLE_OK)
    {
      char* url_ptr;
      curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &url_ptr);
      std::string url(url_ptr);

      if (code == CURLE_GOT_NOTHING)
        this->_raise<curly::EmptyResponse>(url);
      else
        this->_raise<curly::RequestError>(code, url, error_message);
    }
  }

  void
  sched_request::_start()
  {
    auto fn = [&] (int code)
    {
      this->raise_if_ecode(this->_request.config().native_handle(),
		    (CURLcode)code, this->_request.error_string());
      this->_signal();
    };
    this->_request.async_perform(fn);
  }

  void
  sched_request::_abort()
  {
    _signal();
  }
} /* curly */
