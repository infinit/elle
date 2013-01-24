#include <common/common.hh>

#include <boost/algorithm/string/replace.hpp>

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

ELLE_LOG_COMPONENT("infinit.surface.gap.MetricReporter");

namespace surface
{
  namespace gap
  {
    MetricReporter::MetricReporter(std::string const& tag,
                                   std::string const& user)
      : _tag(tag)
      , _user_id(user)
    {}

    MetricReporter::~MetricReporter()
    {
      // Flush cant be done here, call it calls send_data for this object
      // which is pure virtual;
      // this->_flush();
    }

    // Push data directly to server, without enqueuing.
    void
    MetricReporter::publish(std::string const& name,
                            Metric const& metric)
    {
      Metric m = metric;
      m.push_front(std::pair<std::string, std::string>{
                     this->_tag,
                     name});

      this->_send_data(TimeMetricPair(elle::utility::Time::current(), m));
    }


    void
    MetricReporter::store(std::string const& caller, Metric const& metric)
    {
      ELLE_TRACE("Storing new metric %s", caller);

      Metric& m = this->_push(metric);

      // Note that if we want the ability to use initializer list for metric,
      // we can declare it as non const.
      // So we need to push "cd":caller pair after insertion in the map.
      m.push_front(std::pair<std::string, std::string>{
                     this->_tag,
                     caller});

      // Suxx
      if (this->_requests.size() > 10)
        this->_flush();
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

    // Defaultly.
    void
    MetricReporter::update_user(std::string const& id)
    {
      this->_user_id = id;
    }

    MetricReporter::Metric&
    MetricReporter::_push(Metric const& metric)
    {
      this->_requests.push(std::make_pair(elle::utility::Time::current(),
                                          metric));

      return this->_requests.back().second;
    }

    void
    MetricReporter::_flush()
    {
      ELLE_TRACE("Flushing the metrics to server");

      while (this->_requests.size())
      {
        this->_send_data(this->_requests.front());
        this->_requests.pop();
      }
    }

    ServerReporter::ServerReporter(std::string const& tag,
                                   std::string const& default_user,
                                   std::string const& server,
                                   uint16_t port):
      MetricReporter{tag,  default_user},
      _host(server),
      _port(port),
      _server{new elle::HTTPClient{server, port, "MetricReport", true}}
    {
      ELLE_TRACE("conctruct ServerReporter to '%s:%s'", server, port);
    }

    void
    ServerReporter::_flush()
    {
      ELLE_TRACE("server flushing");
      try
      {
        // We reenable the connection. So let push all stored metrics.
        while (this->_fallback_storage.size())
        {
          this->_send_data(this->_fallback_storage.front());

          // Pop if no throw.
          this->_fallback_storage.pop();
        }
      }
      catch(...) // Should be HTTP or Socket exception.
      {
        ELLE_WARN("httpserver didn't respond");
        // Can't do anything in that case, just wait for the next flush to
        // try to reconnect.
      }

      MetricReporter::_flush();
    }

    ServerReporter::~ServerReporter()
    {
      this->_flush();
    }

    void
    ServerReporter::_send_data(MetricReporter::TimeMetricPair const& metric)
    {
      auto request = this->_server->request("POST", "/collect");
      request
        .content_type("application/x-www-form-urlencoded")
        .user_agent("Infinit/1.0 (Linux x86_64)")
        .post_field("dh", "infinit.io")      // Test.
        .post_field("av", "1.0.0")           // Type of interraction.
        .post_field("an", "Infinit")         // Application name.
        .post_field("t", "appview")          // Type of interraction.
        .post_field("cid", this->_user_id)   // Anonymous user.
        .post_field("tid", "UA-31957100-2")  // Tracking ID.
        .post_field("v", "1");               // Api version.

      typedef MetricReporter::Metric::value_type Field;
      std::for_each(metric.second.begin(), metric.second.end(), [&](Field const& f)
                    {
                      std::string value=f.second;
                      size_t pos;
                      // Replace "/" by a %20 (space)
                      while ((pos = value.find('/')) != std::string::npos)
                        value.replace(pos, 1, "%20");
                      request.post_field(f.first, value); });

      _last_sent.Current();
      request.post_field("qt",
                         std::to_string((_last_sent - metric.first).nanoseconds / 1000000));
      try
      {
        request.fire();
      }
      catch(...) // Should be HTTP or Socket exception.
      {
        //XXX: missing some opti here.

        // This means that we tried to send the "stashed" metrics and it failed again...
        if (metric == this->_fallback_storage.front())
          throw;

        ELLE_WARN("httpserver didn't respond. Start fallbacking");
        this->_fallback_storage.push(metric);
      }
    }

    NoConnectionReporter::NoConnectionReporter(std::string const& tag,
                                               std::string const& default_user,
                                               std::string const& path):
      MetricReporter{tag, default_user},
      _file_storage{}
    {
      this->_file_storage.open(path, std::fstream::app);

      if (this->_file_storage.good())
      {
        _last_sent.Current();
        this->_file_storage
          << "==========" << std::endl
          << default_user << std::endl
          << _last_sent.nanoseconds << std::endl
          << "==========" << std::endl;
      }
    }

    NoConnectionReporter::~NoConnectionReporter()
    {
      this->_flush();
    }

    void
    NoConnectionReporter::_send_data(MetricReporter::TimeMetricPair const& metric)
    {
      if (this->_file_storage.good())
      {
        this->_file_storage << metric.first.nanoseconds << std::endl;

        typedef MetricReporter::Metric::value_type Field;
        std::for_each(metric.second.begin(), metric.second.end(), [&](Field const& f)
                      { this->_file_storage << f.first << ":" << f.second << std::endl; });
      }
    }

    namespace metrics
    {
      namespace google
      {

        std::string
        retrieve_id(std::string const& path)
        {
          std::string id = "66666666-6666-6666-6666-66666666";

          std::string id_path = common::metrics::id_path();

          if (elle::os::path::exists(id_path))
          {
            std::ifstream id_file(id_path);

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

          std::string id_path = common::metrics::id_path();
          std::ofstream id_file(id_path);

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
              if (host.empty() ||
                  tag.empty()  ||
                  id.empty())
                throw elle::Exception("google host not specified");

              server.reset(new surface::gap::ServerReporter{
                  tag,
                    id,
                    host,
                    port});
            }

          return *server;
        }

      } // end of google
    } // End of metrics
  } // End of gap
} // End of surface
