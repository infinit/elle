#include <elle/format/hexadecimal.hh>
#include <elle/os/path.hh>

#include <cryptography/Digest.hh>
#include <cryptography/Plain.hh>
#include <cryptography/oneway.hh>

#include "google.hh"


ELLE_LOG_COMPONENT("elle.metrics.google.Service");

namespace elle
{
  namespace metrics
  {
    namespace google
    {
      static std::string pretty_name = "Google";
      //- Service --------------------------------------------------------------
      Service::Service(std::string const& host,
                       uint16_t  port,
                       std::string const& tag,
                       std::string const& id)
        : elle::metrics::Reporter::Service{host, port, tag, id, pretty_name}
      {}

      void
      Service::_send(Reporter::TimeMetricPair const& metric)
      {
        ELLE_TRACE("sending metric");

        // XXX copy constructor with should great idea, cause request base here
        // is always the same, so it could be static.
        elle::Request request = this->_server->request("POST", "/collect");
        request
          .content_type("application/x-www-form-urlencoded")
          .user_agent(elle::metrics::Reporter::user_agent)
          .post_field("dh", "infinit.io")      // Test.
          .post_field("av", elle::metrics::Reporter::version) // Type of interraction.
          .post_field("an", "Infinit")         // Application name.
          .post_field("t", "appview")          // Type of interraction.
          .post_field("cid", this->_user_id)   // Anonymous user.
          .post_field("tid", "UA-31957100-2")  // Tracking ID.
          .post_field("v", "1");               // Api version.

        typedef Reporter::Metric::value_type Field;

        request.post_field(this->_tag, metric.second.at(elle::metrics::Reporter::tag_placeholder));

        for (Field f: metric.second)
        {
          if (f.first != elle::metrics::Reporter::tag_placeholder)
            request.post_field(f.first, f.second);
        };

        _last_sent.Current();
        request.post_field("qt",
                           std::to_string((_last_sent - metric.first).nanoseconds / 1000000));

        request.fire();
      }

      //- Helper ---------------------------------------------------------------
      void
      service(std::string const& host,
              uint16_t port,
              std::string const& tag,
              std::string const& id)
      {
        elle::metrics::reporter().add_service(
          std::unique_ptr<Service>{new Service{host, port, tag, id}});
      }

      std::string
      retrieve_id(std::string const& path)
      {
        std::string id = "66666666-6666-6666-6666-66666666";

        if (elle::os::path::exists(path))
        {
          std::ifstream id_file(path);

          if(!id_file.good())
            return id;

          std::getline(id_file, id);

          id_file.close();
          // should be checked by regex but std regex suxx.
        }

        return id;
      }

      void
      update_id(std::string const& id,
                std::string const& path)
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

        std::ofstream id_file(path);

        if (id_file.good())
        {
          id_file << hashed_id;
          id_file.close();
        }

        elle::metrics::service(pretty_name).update_user(hashed_id);
      }
    } // End of google
  } // End of metrics
} // End of elle.
