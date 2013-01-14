#include <common/common.hh>

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
    namespace
    {
      std::string
      retrieve_id()
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
          return id;
        }

      return id;
      }
    }

    MetricReporter::MetricReporter(std::string const& server,
                                   uint16_t port)
      : _server{new elle::HttpClient{server, port, true}}
      , _user_id(retrieve_id())
    {}

    MetricReporter::~MetricReporter()
    {
      ELLE_ERR(__FUNCTION__);
      this->_send_data();
    }

    void
    MetricReporter::store(std::string const& caller, Metric const& metric)
    {
      ELLE_TRACE("Storing new metric");

      this->_requests.push(metric);

      // Erase "cd" if set in metric.
      // Note that if we want the ability to use initializer list for metric,
      // we can declare it as non const.
      // So we need to push "cd":caller pair after insertion in the map.
      this->_requests.back().insert(std::pair<std::string, std::string>{"cd", caller});

      //XXX: We should bufferize data instead of instant push it.
      this->_send_data();
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

      metric.insert(std::make_pair(key, value));

      this->store(name, metric);
    }

    void
    MetricReporter::store_transaction(std::string const& transaction_id, Metric const& metric)
    {
      ELLE_TRACE("Storing new metric");

      this->_requests.push(metric);

      // Erase "cd" if set in metric.
      // Note that if we want the ability to use initializer list for metric,
      // we can declare it as non const.
      // So we need to push "cd":caller pair after insertion in the map.
      this->_requests.back().insert(std::pair<std::string, std::string>{"ti", transaction_id});

      //XXX: events seem not working.
      // Erase "t":"appview"
      this->_requests.back().insert(std::pair<std::string, std::string>{"t", "event"});

      //XXX: We should bufferize data instead of instant push it.
      this->_send_data();
    }

    // Defaultly.
    void
    MetricReporter::update_user(std::string const& id)
    {
      elle::Buffer id_buffer(reinterpret_cast<elle::Byte const*>(id.data()), id.length());

      infinit::cryptography::Digest digest = infinit::cryptography::oneway::hash(
        infinit::cryptography::Plain(
          elle::WeakBuffer{id_buffer}),
        infinit::cryptography::oneway::Algorithm::sha256);

      std::string hashed_id = elle::format::hexadecimal::encode(
        reinterpret_cast<char const*>(digest.buffer().contents()), digest.buffer().size());

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

      this->_user_id = hashed_id;
    }

    void
    MetricReporter::_send_data()
    {
      ELLE_TRACE("sending metric");

      while(this->_requests.size())
      {
        auto request = this->_server->request("POST", "/collect");
        request
          .content_type("application/x-www-form-urlencoded")
          .post_field("dh", "infinit.io")      // Test.
          .post_field("av", "1.0.0")           // Type of interraction.
          .post_field("an", "Infinit")         // Application name.
          .post_field("t", "appview")          // Type of interraction.
          .post_field("cid", this->_user_id)   // Anonymous user.
          .post_field("tid", "UA-31957100-2")  // Tracking ID.
          .post_field("v", "1");               // Api version.

        auto const& metric = this->_requests.front();

        typedef Metric::value_type Field;
        std::for_each(metric.begin(), metric.end(), [&](Field const& f)
                      { request.post_field(f.first, f.second); });
        request.fire();

        this->_requests.pop();
      }
    }

    namespace metrics
    {
      namespace google
      {
        MetricReporter&
        server()
        {
          static surface::gap::MetricReporter server{
            common::metrics::google_server(),
            80};

          return server;
        }
      }
    }

  }
}
