#include <reactor/exception.hh>
#include <reactor/scheduler.hh>
#include <curly/curly_sched.hh>

namespace curly
{
  sched_request::sched_request(reactor::Scheduler& sched,
                               curly::request_configuration conf):
    Operation{sched},
    _request{sched.io_service(), std::move(conf)}
  {}

  void
  sched_request::raise_if_ecode(CURL* easy,
                                CURLcode code,
                                std::string const& message)
  {
    if (code != CURLE_OK)
    {
      char *url_ptr;
      curl_easy_getinfo(easy, CURLINFO_EFFECTIVE_URL, &url_ptr);

      std::string url{url_ptr};
      std::stringstream ss;
      std::string msg{curl_easy_strerror(code)};
      ss
        << "error code: " << code
        << ": " << msg
        << ": " << url
        << ": " << message;
      this->_raise<elle::Exception>(ss.str());
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
