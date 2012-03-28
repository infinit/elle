#include <reactor/exception.hh>
#include <reactor/network/resolve.hh>
#include <reactor/operation.hh>
#include <reactor/scheduler.hh>

namespace reactor
{
  namespace network
  {
    template <typename Resolver>
    class Resolution: public Operation
    {
      public:
        typedef typename Resolver::endpoint_type EndPoint;
        Resolution(Scheduler& scheduler,
                   const std::string& hostname, const std::string& service)
          : Operation(scheduler)
          , _resolver(scheduler.io_service())
          , _hostname(hostname)
          , _service(service)
          , _end_point()
        {}

        virtual const char* type_name() const
        {
          static const char* name = "name resolution";
          return name;
        }

        EndPoint end_point() const
        {
          return _end_point;
        }

      protected:
        virtual void _abort()
        {
          _resolver.cancel();
          _signal();
        }

        virtual void _start()
        {
          typename Resolver::query query(_hostname, _service);
          _resolver.async_resolve(
            query,
            boost::bind(&Resolution::_wakeup, this, _1, _2));
        }

      private:
        void _wakeup(const boost::system::error_code& error,
                     typename Resolver::iterator it)

        {
          if (error)
            _raise(new Exception(error.message()));
          else
            _end_point = *it;
          _signal();
        }

        Resolver _resolver;
        std::string _hostname;
        std::string _service;
        EndPoint _end_point;
    };

    template <typename Resolver>
    typename Resolver::endpoint_type
    resolve(Scheduler& scheduler,
            const std::string& hostname, const std::string& service)
    {
      Resolution<Resolver> resolution(scheduler, hostname, service);
      resolution.run();
      return resolution.end_point();
    }

    boost::asio::ip::tcp::endpoint
    resolve_tcp(Scheduler& scheduler,
                const std::string& hostname,
                const std::string& service)
    {
      return resolve<boost::asio::ip::tcp::resolver>
        (scheduler, hostname, service);
    }

    boost::asio::ip::udp::endpoint
    resolve_udp(Scheduler& scheduler,
                const std::string& hostname,
                const std::string& service)
    {
      return resolve<boost::asio::ip::udp::resolver>
        (scheduler, hostname, service);
    }
  }
}
