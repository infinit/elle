
#include <elle/concurrency/Scheduler.hh>

#include <reactor/storage.hh>

#include <elle/log.hh>

namespace elle
{
  namespace log
  {
    elle::log::Logger default_logger{
        elle::log::Logger::Level::trace
    };

    namespace detail
    {

      TraceComponent::TraceComponent(std::string const& name)
        : name(name)
      {}

      static ::reactor::LocalStorage<unsigned int> _indent{
          elle::concurrency::scheduler()
      };

      TraceContext::TraceContext(TraceComponent const& component)
        : _component(component)
      { _indent.Get() += 1; }

      TraceContext::~TraceContext()
      { _indent.Get() -= 1; }

      void TraceContext::_send(std::string const& msg)
      {

        std::string align = std::string((_indent.Get() - 1) * 2, ' ');
        default_logger.trace("[" + this->_component.name + "]", align, msg);
      }

    }

  }
}

