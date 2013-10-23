#include <cassert>
#include <iostream>
#include <stdexcept>

#include <curl/curl.h>

#include <boost/asio.hpp>

#include <elle/log.hh>
#include <elle/memory.hh>

#include <reactor/http/Client.hh>
#include <reactor/http/Request.hh>
#include <reactor/http/Service.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.http.Client");

namespace reactor
{
  namespace http
  {
    class Client::Impl
    {
    public:
      Impl():
        _curl(boost::asio::use_service<Service>(
                Scheduler::scheduler()->io_service())),
        _share(curl_share_init(), &curl_share_cleanup)
      {
        if (!this->_share)
          throw elle::Exception("unable to initialize CURL shared");
        curl_share_setopt(this->_share.get(),
                          CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
      }

      ~Impl()
      {}

    private:
      friend class Client;
      Service& _curl;
      ELLE_ATTRIBUTE(elle::generic_unique_ptr<CURLSH>, share);
    };

    Client::Client():
      _impl(new Impl)
    {}

    Client::~Client()
    {
      delete this->_impl;
    }

    CURL*
    handle_from_request(Request const& r);

    elle::Buffer
    Client::get(std::string const& url,
                Request::Configuration conf)
    {
      Request r = this->request(url, reactor::http::Method::GET, conf);
      return r.response();
    }

    CURL*
    handle_from_request(Request const& r);

    void
    Client::_register(Request const& request)
    {
      auto res = curl_easy_setopt(handle_from_request(request),
                                  CURLOPT_SHARE, this->_impl->_share.get());
      if (res != CURLE_OK)
        throw elle::Exception(elle::sprintf("unable to set cookie jar: %s",
                                            curl_easy_strerror(res)));
    }
  }
}
