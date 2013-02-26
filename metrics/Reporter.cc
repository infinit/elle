#include <common/common.hh>

#include <boost/algorithm/string/replace.hpp>

#include <elle/serialize/extract.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/BinaryArchive.hh>
#include <elle/serialize/PairSerializer.hxx>
#include <elle/serialize/ListSerializer.hxx>

#include <elle/utility/Time.hh>
#include <elle/Buffer.hh>
#include <elle/format/hexadecimal.hh>
#include <cryptography/Digest.hh>
#include <cryptography/Plain.hh>
#include <cryptography/oneway.hh>

#include "Reporter.hh"

#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <thread>
#include <mutex>

ELLE_LOG_COMPONENT("elle.metrics.Reporter");

namespace elle
{
  namespace metrics
  {
    std::string Reporter::tag_placeholder{"_tag"};
    std::string Reporter::version =
      elle::sprintf("%s.%s", INFINIT_VERSION_MAJOR, INFINIT_VERSION_MINOR);
    std::string Reporter::user_agent = elle::sprintf("Infinit/%s (%s)",
                                                     Reporter::version,
#ifdef INFINIT_LINUX
                                                  "Linux x86_64");
#elif INFINIT_MACOSX
               // XXX[10.7: should adapt to any MacOS X version]
                                                  "Mac OS X 10.7");
#else
# warning "machine not supported"
#endif

    //- Service ----------------------------------------------------------------
    Reporter::Service::Service(std::string const& host,
                               uint16_t port,
                               std::string const& tag,
                               std::string const& user,
                               std::string const& pretty_name)
      : _tag{tag}
      , _user_id{user}
      , _server{new elle::HTTPClient{host, port, Reporter::user_agent}}
      , name{pretty_name}
    {}

    //- Reporter ---------------------------------------------------------------
    Reporter::Reporter()
      : _flusher_sched{}
      , _fallback_stream{common::metrics::fallback_path()}
    {
      this->_keep_alive.reset(
        new boost::asio::io_service::work(_flusher_sched.io_service()));
      this->_run_thread.reset(new std::thread{[this] { this->_flusher_sched.run(); }});
    }

    Reporter::~Reporter()
    {
      this->_keep_alive.reset();
      if (this->_run_thread->joinable())
        this->_run_thread->join();
    }

    void
    Reporter::store(std::string const& caller, Metric const& metric)
    {
      ELLE_TRACE("Storing new metric %s", caller);

      // Note that if we want the ability to use initializer list for metric,
      // we can't declare it as non const..
      Metric& m = const_cast<Metric &>(metric);
      m.emplace(tag_placeholder, caller);

      this->store(TimeMetricPair(elle::utility::Time::current(), m));
    }

    void
    Reporter::store(std::string const& caller)
    {
      this->store(caller, {});
    }

    void
    Reporter::store(std::string const& name,
                          std::string const& key,
                          std::string const& value)
    {
      Metric metric;
      metric.emplace(key, value);
      this->store(name, metric);
    }

    void
    Reporter::store(TimeMetricPair const& metric)
    {
      for (auto& serv: _services)
      {
        try
        {
          serv.second->_send(metric);
        }
        catch (std::runtime_error const& e)
        {
          ELLE_ERR("something went wrong while storing metric: %s", e.what());
          this->_fallback(metric);
        }
      }
    }

    void
    Reporter::_fallback(TimeMetricPair const& metric)
    {}

    void
    Reporter::add_service(std::unique_ptr<Service> service)
    {
      this->_services.emplace(service->name, std::move(service));
    }

    Reporter::Service&
    Reporter::service(std::string const& name)
    {
      auto& service = _services.at(name);

      ELLE_ASSERT(service != nullptr);

      return *service;
    }

    Reporter&
    reporter()
    {
      static Reporter reporter;

      return reporter;
    }
  } // End of metric
} // End of elle
