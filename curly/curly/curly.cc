#include <curly/curly.hh>

#include <elle/Exception.hh>

#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>

namespace curly
{
  void
  _throw_if_ecode(CURL* easy,
                  CURLcode code,
                  std::string const& error_message)
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
        << ": " << error_message;
      throw elle::Exception{ss.str()};
    }
  }

  void
  curl_slist_deleter::operator ()(struct curl_slist* p)
  {
    curl_slist_free_all(p);
  }

  void
  curl_easy_deleter::operator ()(CURL* easy_handle)
  {
    curl_easy_cleanup(easy_handle);
  }

  request_configuration::request_configuration():
    _easy_handle{curl_easy_init(), curl_easy_deleter{}}
  {}

  void
  request_configuration::url(std::string const& url_)
  {
    this->option(CURLOPT_URL, url_.c_str());
  }

  void
  request_configuration::user_agent(std::string const& ua)
  {
    this->option(CURLOPT_USERAGENT, ua.c_str());
  }

  void
  request_configuration::cookie(std::string const& cookie)
  {
    this->option(CURLOPT_COOKIE, cookie.c_str());
  }

  void
  request_configuration::referer(std::string const& referer)
  {
    this->option(CURLOPT_REFERER, referer.c_str());
  }

  CURL*
  request_configuration::native_handle()
  {
    return _easy_handle.get();
  }

  void
  request_configuration::header(std::string const& name,
                                std::string const& value)
  {
    std::string header = name + ": " + value;
    this->_headers.reset(
      curl_slist_append(this->_headers.release(), header.c_str()));
    this->option(CURLOPT_HTTPHEADER, this->_headers.get());
  }

  void
  request_configuration::headers(std::map<std::string, std::string> const& m)
  {
    for (auto const& pair: m)
      this->header(pair.first, pair.second);
  }

  void
  request_configuration::output(std::ostream& out)
  {
    this->_output = &out;
  }

  void
  request_configuration::input(std::istream& in)
  {
    this->_input = &in;
  }

  int
  request::code() const
  {
    return this->_response_code;
  }

  size_t
  request::write_helper(char* data,
                        size_t size,
                        size_t nmemb,
                        void* userptr)
  {
    request* _this = reinterpret_cast<request*>(userptr);
    ssize_t total_size = size * nmemb;
    if (total_size > 0)
      (*_this->_config._output) << std::string{data, data + total_size};
    return size * nmemb;
  }

  size_t
  request::read_helper(char* data,
                       size_t size,
                       size_t nmemb,
                       void* userptr)
  {
    request* _this = reinterpret_cast<request*>(userptr);
    _this->_config._input->read(data, size * nmemb);
    return _this->_config._input->gcount();
  }

  request::request(request_configuration c):
    _config(std::move(c))
  {
    char error[CURL_ERROR_SIZE] = {};

    _config.option(CURLOPT_ERRORBUFFER, error);
    _config.option(CURLOPT_FAILONERROR, 1);
    if (this->_config._output)
    {
      _config.option(CURLOPT_WRITEFUNCTION, &request::write_helper);
      _config.option(CURLOPT_WRITEDATA, this);
    }
    if (this->_config._input != nullptr)
    {
      _config.option(CURLOPT_READFUNCTION, &request::read_helper);
      _config.option(CURLOPT_READDATA, this);
    }

    auto ec = curl_easy_perform(_config.native_handle());
    this->info(CURLINFO_RESPONSE_CODE, &this->_response_code);
    _throw_if_ecode(this->_config.native_handle(), ec, std::string{error});
  }

  std::string
  request::url()
  {
    char *url;

    info(CURLINFO_EFFECTIVE_URL, &url);
    return std::string{url};
  }

  std::chrono::duration<double>
  request::time()
  {
    double time;

    info(CURLINFO_TOTAL_TIME, &time);
    return std::chrono::duration<double>{time};
  }

  std::string
  get(std::string const& url)
  {
    return method_simple<CURLOPT_HTTPGET>(url);
  }

  void
  get(std::string const& url,
      std::ostream& out)
  {
    method_out<CURLOPT_HTTPGET>(url, out);
  }

  std::string
  post(std::string const& url,
       std::istream& in)
  {
    return method_in<CURLOPT_POST>(url, in);
  }

  void
  post(std::string const& url,
       std::istream& in,
       std::ostream& out)
  {
    method_inout<CURLOPT_POST>(url, in, out);
  }

  std::string
  put(std::string const& url,
      std::istream& in)
  {
    return method_in<CURLOPT_UPLOAD>(url, in);
  }

  void
  put(std::string const& url,
      std::istream& in,
      std::ostream& out)
  {
    method_inout<CURLOPT_UPLOAD>(url, in, out);
  }

  request_configuration
  make_get()
  {
    return make_method<CURLOPT_HTTPGET>();
  }

  request_configuration
  make_post()
  {
    return make_method<CURLOPT_POST>();
  }

  request_configuration
  make_put()
  {
    return make_method<CURLOPT_UPLOAD>();
  }
} /* curly */
