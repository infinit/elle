#include "curly.hh"

#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>

namespace curly
{
  void
  request_configuration::curl_slist_deleter::operator ()(struct curl_slist* p)
  {
    curl_slist_free_all(p);
  }

  void
  request_configuration::curl_easy_deleter::operator ()(CURL* easy_handle)
  {
    curl_easy_cleanup(easy_handle);
  }

  request_configuration::request_configuration():
    _easy_handle{curl_easy_init()}
  {
  }

  void
  request_configuration::url(std::string const& url_)
  {
    this->option(CURLOPT_URL, url_.c_str());
  }

  CURL*
  request_configuration::native_handle()
  {
    return _easy_handle.get();
  }
  
  std::string
  request_configuration::header(std::pair<std::string, std::string> const&p)
  {
    std::stringstream ss;
    ss << p.first << ": " << p.second;
    return ss.str();
  }

  void
  request_configuration::headers(std::map<std::string, std::string> const &m)
  {
    struct curl_slist *headers = nullptr;

    for (auto const& p: m)
      headers = curl_slist_append(headers, header(p).c_str());
    this->option(CURLOPT_HTTPHEADER, headers);
    this->_header_list.reset(headers);
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

  size_t
  request::write_helper(char* data,
                        size_t size, size_t nmemb,
                        void* userptr)
  {
    request* _this = reinterpret_cast<request*>(userptr);
    (*_this->_config._output) << std::string{data, data + (size * nmemb)};
    return size * nmemb;
  }

  size_t
  request::read_helper(char* data,
                       size_t size, size_t nmemb,
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
    int res = curl_easy_perform(_config.native_handle());
    this->info(CURLINFO_RESPONSE_CODE, &this->_response_code);
    if (res != 0)
    {
      // XXX change to elle::sprint
      throw std::runtime_error{
        std::to_string(this->_response_code) + ": " + std::string{error}
      };
    }
  }
} /* curly */
