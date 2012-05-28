
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
      static ::reactor::LocalStorage<unsigned int> _indent{
          elle::concurrency::scheduler()
      };

      static unsigned int _max_component_size = 0;

      TraceComponent::TraceComponent(std::string const& name)
        : name(name)
      {
        if (name.size() > _max_component_size)
          _max_component_size = name.size();
      }


      TraceContext::TraceContext(TraceComponent const& component)
        : _component(component)
      { _indent.Get() += 1; }

      TraceContext::~TraceContext()
      { _indent.Get() -= 1; }

      void TraceContext::_send(std::string const& msg)
      {
        std::string align = std::string((_indent.Get() - 1) * 2, ' ');
        assert(this->_component.name.size() <= _max_component_size);
        unsigned int pad = _max_component_size - this->_component.name.size();
        std::string s = (
          "[" + std::string(pad / 2, ' ') +
          this->_component.name +
          std::string(pad / 2 + pad % 2, ' ') +
          "]"
        );
        default_logger.trace(s, align, msg);
      }

    }

  }
}

