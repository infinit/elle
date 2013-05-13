#pragma once
#ifndef CURLY_HH_
#define CURLY_HH_

#include <map>
#include <memory>
#include <sstream>
#include <iostream>
#include <curl/curl.h>

typedef void CURL;


namespace curly
{
  class request_configuration
  {
  private:
    // The request class will fire the request: gives acces to _easy_handle.
    friend class request;
    class curl_easy_deleter
    {
    public:
      curl_easy_deleter() = default;
      void 
      operator ()(CURL* ptr);
    };
    class curl_slist_deleter
    {
    public:
      curl_slist_deleter() = default;
      void 
      operator ()(struct curl_slist* ptr);
    };

    std::unique_ptr<CURL, curl_easy_deleter> _easy_handle;
    std::unique_ptr<struct curl_slist, curl_slist_deleter> _header_list = nullptr;

    std::ostream* _output = nullptr;
    std::istream* _input = nullptr;

    CURL*
    native_handle();

    std::string
    header(std::pair<std::string, std::string> const&p);
  public:
    request_configuration();
    request_configuration(request_configuration&&) = default;
    ~request_configuration() = default;

    void
    url(std::string const&);

    void
    output(std::ostream& out);

    void
    input(std::istream& out);

    void
    headers(std::map<std::string, std::string> const &m);

    template <typename T>
    int
    option(CURLoption opt, T&& param)
    {
      return curl_easy_setopt(_easy_handle.get(),
                              opt, std::forward<T>(param));
    }
  };

  class request
  {
  private:
    request_configuration _config;
    int _response_code = -1;

    static
    size_t
    write_helper(char *ptr,
                 size_t size, size_t nmemb,
                 void *userptr);
    
    static
    size_t
    read_helper(char *ptr,
                size_t size, size_t nmemb,
                void *userptr);
  public:
    request(request_configuration c);

    template <typename T>
    int
    info(CURLINFO info, T&& param)
    {
      return curl_easy_getinfo(this->_config.native_handle(), info,
                               std::forward<T>(param));
    }
  };

  template <CURLoption Opt>
  void
  method_in_v(std::string const& url, std::istream& in)
  {
    curly::request_configuration c;
    
    c.option(CURLOPT_VERBOSE, 1);
    c.option(Opt, 1);

    c.url(url);
    c.input(in);
    curly::request req(std::move(c));
  }

  template <CURLoption Opt>
  std::string
  method_in(std::string const& url, std::istream& in)
  {
    std::stringstream ss;
    curly::request_configuration c;
    
    c.option(CURLOPT_VERBOSE, 1);
    c.option(Opt, 1);

    c.url(url);
    c.input(in);
    c.output(ss);
    curly::request req(std::move(c));
    return ss.str();
  }

  //template <CURLoption Opt>
  //void
  //method_out(std::string const& url, std::ostream& out)
  //{
  //  curly::request_configuration c;
  //  
  //  c.option(CURLOPT_VERBOSE, 1);
  //  c.option(Opt, 1);

  //  c.url(url);
  //  c.output(out);
  //  curly::request req(std::move(c));
  //}

  //template <CURLoption Opt>
  //void
  //method_inout(std::string const& url, std::istream& in, std::ostream& out)
  //{
  //  curly::request_configuration c;
  //  
  //  c.option(CURLOPT_VERBOSE, 1);
  //  c.option(Opt, 1);

  //  c.url(url);
  //  c.input(in);
  //  c.output(out);
  //  curly::request req(std::move(c));
  //}

  //template <CURLoption Opt>
  //std::string
  //method_simple(std::string const& url)
  //{
  //  std::stringstream ss;
  //  curly::request_configuration c;
  //  
  //  c.option(CURLOPT_VERBOSE, 1);
  //  c.option(Opt, 1);

  //  c.url(url);
  //  c.output(ss);
  //  curly::request req(std::move(c));
  //  return ss.str();
  //}

  //static auto get = method_simple<CURLOPT_HTTPGET>;
  //static auto iget = method_in<CURLOPT_HTTPGET>;
  //static auto oget = method_out<CURLOPT_HTTPGET>;
  //static auto ioget = method_inout<CURLOPT_HTTPGET>;

  //static auto post = method_simple<CURLOPT_POST>;
  //static auto ipost = method_in<CURLOPT_POST>;
  //static auto opost = method_out<CURLOPT_POST>;
  //static auto iopost = method_inout<CURLOPT_POST>;

  //static auto put = method_in_v<CURLOPT_UPLOAD>;
  //static auto iput = method_in<CURLOPT_UPLOAD>;
  //static auto oput = method_out<CURLOPT_UPLOAD>;
  //static auto ioput = method_inout<CURLOPT_UPLOAD>;
} /* curly */


#endif /* end of include guard: CURLY_HH_ */
