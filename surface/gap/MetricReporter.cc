#include <common/common.hh>

#include <boost/algorithm/string/replace.hpp>

#include <elle/serialize/extract.hh>
#include <elle/serialize/insert.hh>
#include <elle/serialize/BinaryArchive.hh>
#include <elle/serialize/PairSerializer.hxx>
#include <elle/serialize/ListSerializer.hxx>


#include <elle/os/path.hh>
#include <elle/utility/Time.hh>
#include <elle/Buffer.hh>
#include <elle/format/hexadecimal.hh>
#include <cryptography/Digest.hh>
#include <cryptography/Plain.hh>
#include <cryptography/oneway.hh>

#include "MetricReporter.hh"

#include <fstream>
#include <iostream>
#include <string>
#include <regex>
#include <thread>
#include <mutex>

ELLE_LOG_COMPONENT("infinit.surface.gap.MetricReporter");

namespace surface
{
  namespace gap
  {

    MetricReporter::MetricReporter(std::string const& tag,
                                   std::string const& user,
                                   std::string const& host,
                                   uint16_t port)
      : _tag{tag}
      , _user_id{user}
      , _flusher_sched{}
      , _server{new elle::HTTPClient{host, port, "MetricReport"}}
      , _fallback_stream{common::metrics::fallback_path()}
    {
      this->_keep_alive.reset(
        new boost::asio::io_service::work(_flusher_sched.io_service()));
      this->_run_thread.reset(new std::thread{[this] { this->_flusher_sched.run(); }});
    }

    MetricReporter::~MetricReporter()
    {
      this->_keep_alive.reset();
      if (this->_run_thread->joinable())
        this->_run_thread->join();
    }

    void
    MetricReporter::store(std::string const& caller, Metric const& metric)
    {
      ELLE_TRACE("Storing new metric %s", caller);

      // Note that if we want the ability to use initializer list for metric,
      // we can't declare it as non const..
      Metric& m = const_cast<Metric &>(metric);
      m.push_front(std::pair<std::string, std::string>{this->_tag, caller});

      this->store(TimeMetricPair(elle::utility::Time::current(), m));
    }

    void
    MetricReporter::store(std::string const& caller)
    {
      this->store(caller, {});
    }

    void
    MetricReporter::store(std::string const& name,
                          std::string const& key,
                          std::string const& value)
    {
      Metric metric;
      metric.push_back(std::make_pair(key, value));
      this->store(name, metric);
    }

    void
    MetricReporter::store(TimeMetricPair const& metric)
    {
      try
      {
        this->_send_data(metric);
      }
      catch (std::runtime_error const& e)
      {
        ELLE_ERR("something went wrong while storing metric: %s", e.what());
        this->_fallback(metric);
      }
    }

    void
    MetricReporter::_fallback(TimeMetricPair const& metric)
    {}

    void
    MetricReporter::update_user(std::string const& id)
    {
      this->_user_id = id;
    }

    void
    MetricReporter::_send_data(MetricReporter::TimeMetricPair const& metric)
    {
      ELLE_TRACE("sending metric");
      static std::string version =
        elle::sprintf("%s.%s", INFINIT_VERSION_MAJOR, INFINIT_VERSION_MINOR);
      static std::string user_agent = elle::sprintf("Infinit/%s (%s)",
                                                    version,
#ifdef INFINIT_LINUX
                                                    "Linux x86_64");
#elif INFINIT_MACOSX
                                                    // XXX[10.7: should adapt to any MacOS X version]
                                                    "Mac OS X 10.7");
#else
# warning "machine not supported"
#endif
      // XXX copy constructor with should great idea, cause request base here
      // is always the same, so it could be static.
      elle::Request request = this->_server->request("POST", "/collect");
      request
        .content_type("application/x-www-form-urlencoded")
        .user_agent(user_agent)
        .post_field("dh", "infinit.io")      // Test.
        .post_field("av", version)           // Type of interraction.
        .post_field("an", "Infinit")         // Application name.
        .post_field("t", "appview")          // Type of interraction.
        .post_field("cid", this->_user_id)   // Anonymous user.
        .post_field("tid", "UA-31957100-2")  // Tracking ID.
        .post_field("v", "1");               // Api version.

      typedef MetricReporter::Metric::value_type Field;
      for (Field f: metric.second)
      {
        // std::string value = f.second;
        // size_t pos;
        // // Replace "/" by a :
        // while ((pos = value.find('/')) != std::string::npos)
        //   value.replace(pos, 1, ":");
        request.post_field(f.first, f.second);
      };
      _last_sent.Current();
      request.post_field("qt",
                         std::to_string((_last_sent - metric.first).nanoseconds / 1000000));

      request.fire();
    }

    namespace metrics
    {
      namespace google
      {

        std::string
        retrieve_id(std::string const& path)
        {
          std::string id = "66666666-6666-6666-6666-66666666";

          if (elle::os::path::exists(common::metrics::id_path()))
          {
            std::ifstream id_file(common::metrics::id_path());

            if(!id_file.good())
              return id;

            std::getline(id_file, id);

            id_file.close();
            // should be checked by regex but std regex suxx.
          }

          return id;
        }

        void
        update_id(std::string const& id)
        {
          elle::Buffer id_buffer(reinterpret_cast<elle::Byte const*>(id.data()), id.length());

          infinit::cryptography::Digest digest = infinit::cryptography::oneway::hash(
            infinit::cryptography::Plain(
              elle::WeakBuffer{id_buffer}),
            infinit::cryptography::oneway::Algorithm::sha256);

          std::string hashed_id =
            elle::format::hexadecimal::encode(digest.buffer());

          ELLE_DEBUG("hashed id: %s", hashed_id);

          // Google user id must have the following format:
          // xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxx
          // [[:alpha:]]{8}-[[:alpha:]]{4}-[[:alpha:]]{4}-[[:alpha:]]{4}-[[:alpha:]]{12}
          if (hashed_id.length() < 32)
            hashed_id.append(32 - hashed_id.length(), '6');
          else
            hashed_id.erase(32, std::string::npos);

          hashed_id
            .insert(20, "-")
            .insert(16, "-")
            .insert(12, "-")
            .insert(8, "-");

          ELLE_DEBUG("hashed id: %s", hashed_id);

          std::ofstream id_file(common::metrics::id_path());

          if (id_file.good())
          {
            id_file << hashed_id;
            id_file.close();
          }

          server().update_user(hashed_id);
        }

        MetricReporter&
        server(std::string const& host,
               uint16_t port,
               std::string const& tag,
               std::string const& id)
        {
          static std::unique_ptr<surface::gap::MetricReporter> server;
          if (!server)
          {
            if (host.empty() || tag.empty()  || id.empty())
              throw elle::Exception("google host not specified");

            server.reset(new surface::gap::MetricReporter{
                tag, id, host, port});
          }

          return *server;
        }

      } // end of google
    } // End of metrics
  } // End of gap
} // End of surface
