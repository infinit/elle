#include <cassert>
#include <iostream>
#include <stdexcept>

#include <curl/curl.h>

#include <boost/asio.hpp>

#include <elle/log.hh>
#include <elle/memory.hh>
#include <elle/reactor/http/Client.hh>
#include <elle/reactor/http/RequestImpl.hh>
#include <elle/reactor/http/Service.hh>
#include <elle/reactor/http/exceptions.hh>
#include <elle/reactor/scheduler.hh>

ELLE_LOG_COMPONENT("elle.reactor.http.Client");

namespace elle
{
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
            throw std::bad_alloc();
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

      Client::Client(std::string const& user_agent):
        _impl(new Impl),
        _user_agent(user_agent)
      {}

      Client::~Client()
      {
        delete this->_impl;
      }

      elle::Buffer
      Client::get(std::string const& url,
                  Request::Configuration conf)
      {
        Request r = this->request(url, reactor::http::Method::GET, conf);
        return r.response();
      }

      void
      Client::_register(Request const& request)
      {
        ELLE_TRACE_SCOPE("%s: register %s", *this, request);
        {
          auto res = curl_easy_setopt(request._impl->_handle,
                                      CURLOPT_SHARE, this->_impl->_share.get());
          if (res != CURLE_OK)
            throw RequestError(request.url(),
                               elle::sprintf("unable to set cookie jar: %s",
                                             curl_easy_strerror(res)));
        }

        {
          auto res = curl_easy_setopt(request._impl->_handle,
                                      CURLOPT_USERAGENT,
                                      this->_user_agent.c_str());
          if (res != CURLE_OK)
            throw RequestError(request.url(),
                               elle::sprintf("unable to set user agent: %s",
                                             curl_easy_strerror(res)));
        }
      }

      /*--------.
      | Cookies |
      `--------*/

      Request::Configuration::Cookies
      Client::cookies() const
      {
        elle::generic_unique_ptr<CURL> handle(curl_easy_init(),
                                              &curl_easy_cleanup);
        if (!handle)
          throw elle::Exception("unable to initialize request");
        auto res = curl_easy_setopt(handle.get(),
                                    CURLOPT_SHARE, this->_impl->_share.get());
        if (res != CURLE_OK)
          throw elle::Exception("unable to set cookie jar");
        return Request::Impl::cookies(handle.get());
      }
    }
  }
}
