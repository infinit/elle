#include <elle/log.hh>

#include <reactor/network/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>

ELLE_LOG_COMPONENT("reactor.network.resolve");

namespace reactor
{
  namespace network
  {
    template <typename Resolver>
    class Resolution: public Operation
    {
      public:
        typedef typename Resolver::endpoint_type EndPoint;
        Resolution(const std::string& hostname, const std::string& service)
          : Operation(*reactor::Scheduler::scheduler())
          , _resolver(reactor::Scheduler::scheduler()->io_service())
          , _hostname(hostname)
          , _service(service)
          , _end_point()
        {}

        virtual
        void
        print(std::ostream& stream) const override
        {
          stream << "resolution of " << this->_hostname << ":" << this->_service;
        }

        virtual
        const
        char*
        type_name() const
        {
          static const char* name = "name resolution";
          return name;
        }

      protected:
        virtual
        void
        _abort()
        {
          this->_resolver.cancel();
          this->_signal();
        }

        virtual
        void
        _start()
        {
          ELLE_TRACE("resolve %s:%s", this->_hostname, this->_service);
          typename Resolver::query query(this->_hostname, this->_service);
          this->_resolver.async_resolve(
            query,
            boost::bind(&Resolution::_wakeup, this, _1, _2));
        }

      private:
        void
        _wakeup(const boost::system::error_code& error,
                typename Resolver::iterator it)

        {
          if (error)
            _raise<ResolutionError>(this->_hostname, error.message());
          else
            this->_end_point = *it;
          this->_signal();
        }

        ELLE_ATTRIBUTE(Resolver, resolver);
        ELLE_ATTRIBUTE(std::string, hostname);
        ELLE_ATTRIBUTE(std::string, service);
        ELLE_ATTRIBUTE_R(EndPoint, end_point);
    };

    template <typename Resolver>
    typename Resolver::endpoint_type
    resolve(const std::string& hostname, const std::string& service)
    {
      Resolution<Resolver> resolution(hostname, service);
      resolution.run();
      return resolution.end_point();
    }

    boost::asio::ip::tcp::endpoint
    resolve_tcp(const std::string& hostname, const std::string& service)
    {
      return resolve<boost::asio::ip::tcp::resolver>(hostname, service);
    }

    boost::asio::ip::udp::endpoint
    resolve_udp(const std::string& hostname, const std::string& service)
    {
      return resolve<boost::asio::ip::udp::resolver>(hostname, service);
    }
  }
}
