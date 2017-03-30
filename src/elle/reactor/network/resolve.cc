#include <elle/reactor/network/resolve.hh>

#include <utility>

#include <elle/log.hh>
#include <elle/printf.hh>

#include <elle/reactor/network/Error.hh>
#include <elle/reactor/Operation.hh>
#include <elle/reactor/scheduler.hh>

ELLE_LOG_COMPONENT("elle.reactor.network.resolve");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      namespace
      {
        /// DNS lookup implementation.
        template <typename Protocol>
        class Resolution: public Operation
        {
        public:
          using Super = Operation;
          using Resolver = typename Protocol::resolver;
          using EndPoint = typename Protocol::resolver::endpoint_type;
          using EndPoints = std::vector<EndPoint>;
          Resolution(std::string hostname, std::string service,
                     bool ipv4_only)
            : Super{*reactor::Scheduler::scheduler()}
            , _resolver{reactor::Scheduler::scheduler()->io_service()}
            , _canceled{false}
            , _hostname{std::move(hostname)}
            , _service{std::move(service)}
            , _end_points{}
            , _ipv4_only{ipv4_only}
          {}

          void
          print(std::ostream& stream) const override
          {
            elle::fprintf(stream,
                          "resolution of %s: %s", this->_hostname, this->_service);
          }

          /// Exception thrown when DNS invited us to retry.
          class TryAgain
            : public Exception
          {
          public:
            using Super = Exception;
            TryAgain()
              : Super{"Host not found (non-authoritative), try again later"}
            {}
          };

        protected:
          void
          _abort() override
          {
            ELLE_TRACE_SCOPE("%s: abort", *this);
            this->_canceled = true;
            this->_resolver.cancel();
            reactor::wait(*this);
          }

          void
          _start() override
          {
            ELLE_TRACE("resolve %s:%s", this->_hostname, this->_service);
            using Query = typename Resolver::query;
            auto const query =
              this->ipv4_only()
              ? Query(Protocol::v4(), this->_hostname, this->_service)
              : Query(this->_hostname, this->_service);
            this->_resolver.async_resolve(
              query,
              [this](boost::system::error_code const& error,
                     typename Resolver::iterator it)
              {
                this->_wakeup(error, it);
              });
          }

        private:
          void
          _wakeup(boost::system::error_code const& error,
                  typename Resolver::iterator it)
          {
            auto const end
              = typename Resolver::iterator::basic_resolver_iterator();
            if (this->_canceled)
            {
              ELLE_TRACE_SCOPE("%s: canceled", *this);
            }
            else if (error == boost::asio::error::host_not_found_try_again
                     || getenv("ELLE_REACTOR_RESOLVE_TRY_AGAIN"))
            {
              ELLE_TRACE_SCOPE("%s: ended with error: %s", *this, error.message());
              this->_raise<TryAgain>();
            }
            else if (error)
            {
              ELLE_TRACE_SCOPE("%s: ended with error: %s", *this, error.message());
              this->_raise<ResolutionError>(this->_hostname, error.message());
            }
            else if (it == end)
            {
              // From the Boost documentation:
              //   A successful resolve operation is guaranteed to pass at least
              //   one entry to the handler.
              // This is false on wine.
              ELLE_TRACE_SCOPE(
                "%s: ended with no error but an empty address list", *this);
              this->_raise<ResolutionError>(
                this->_hostname, "host not found: address list is empty");
            }
            else
            {
              ELLE_TRACE_SCOPE("%s: ended", *this);
              for (; it != end; ++it)
              {
                this->_end_points.emplace_back(*it);
                ELLE_DEBUG("%s: endpoint address: %s",
                           *this, this->_end_points.back().address().to_string());
              }
            }
            this->done();
          }

          ELLE_ATTRIBUTE(Resolver, resolver);
          ELLE_ATTRIBUTE(bool, canceled);
          ELLE_ATTRIBUTE(std::string, hostname);
          ELLE_ATTRIBUTE(std::string, service);
          ELLE_ATTRIBUTE_R(EndPoints, end_points);
          ELLE_ATTRIBUTE_R(bool, ipv4_only);
        };

        template <typename Protocol>
        std::vector<typename Protocol::resolver::endpoint_type>
        resolve(std::string const& hostname,
                std::string const& service,
                ResolveOptions opt)
        {
          while (0 < opt.num_attempts)
            try
            {
              Resolution<Protocol> resolution(hostname, service, opt.ipv4_only);
              resolution.run();
              return resolution.end_points();
            }
            catch (typename Resolution<Protocol>::TryAgain)
            {
              ELLE_DUMP("trying again to resolve %s:%s", hostname, service);
              --opt.num_attempts;
            }

          throw ResolutionError(
            hostname,
            sprintf("host not found: too many attempts: %s",
                    make_error_code(boost::asio::error::host_not_found_try_again).message()));
        }

        /// "www.infinit.sh:80" -> pair("www.infinit.sh", "80").
        auto
        host_port(std::string const& repr)
        {
          auto const sep = repr.find_last_of(':');
          if (sep == std::string::npos || sep == repr.length())
            elle::err("invalid endpoint: %s", repr);
          return std::make_pair(repr.substr(0, sep), repr.substr(sep + 1));
        }
      }

      /*------.
      | tcp.  |
      `------*/

      std::vector<boost::asio::ip::tcp::endpoint>
      resolve_tcp(std::string const& hostname,
                  std::string const& service,
                  ResolveOptions opt)
      {
        return resolve<boost::asio::ip::tcp>(hostname, service, opt);
      }

      std::vector<boost::asio::ip::tcp::endpoint>
      resolve_tcp(std::string const& hostname,
                  int port,
                  ResolveOptions opt)
      {
        return resolve<boost::asio::ip::tcp>(hostname, std::to_string(port), opt);
      }

      std::vector<boost::asio::ip::tcp::endpoint>
      resolve_tcp_repr(std::string const& repr, ResolveOptions opt)
      {
        auto hp = host_port(repr);
        return resolve<boost::asio::ip::tcp>(std::get<0>(hp), std::get<1>(hp), opt);
      }

      /*------.
      | udp.  |
      `------*/

      std::vector<boost::asio::ip::udp::endpoint>
      resolve_udp(std::string const& hostname,
                  std::string const& service,
                  ResolveOptions opt)
      {
        return resolve<boost::asio::ip::udp>(hostname, service, opt);
      }

      std::vector<boost::asio::ip::udp::endpoint>
      resolve_udp(std::string const& hostname,
                  int port,
                  ResolveOptions opt)
      {
        return resolve<boost::asio::ip::udp>(hostname, std::to_string(port), opt);
      }

      std::vector<boost::asio::ip::udp::endpoint>
      resolve_udp_repr(std::string const& repr,
                       ResolveOptions opt)
      {
        auto hp = host_port(repr);
        return resolve<boost::asio::ip::udp>(std::get<0>(hp), std::get<1>(hp), opt);
      }
    }
  }
}
