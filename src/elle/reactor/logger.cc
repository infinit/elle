#include <elle/Plugin.hh>
#include <elle/log/Logger.hh>
#include <elle/os/environ.hh>
#include <elle/reactor/storage.hh>
#include <utility>

namespace elle
{
  namespace reactor
  {
    class Indentation
      : public elle::log::Indentation
    {
    public:
      using Factory = elle::log::Indenter::Factory;
      Indentation(Factory factory)
        : _factory(std::move(factory))
        , _indentations()
      {}

      int&
      indentation() override
      {
        return this->_indentation()->indentation();
      }

      void
      indent() override
      {
        return this->_indentation()->indent();
      }

      void
      unindent() override
      {
        return this->_indentation()->unindent();
      }

      std::unique_ptr<elle::log::Indentation>
      clone() override
      {
        return std::make_unique<Indentation>(_factory);
      }

    private:
      std::unique_ptr<elle::log::Indentation>&
      _indentation()
      {
        std::unique_ptr<elle::log::Indentation>& idt = _indentations.get();
        if (!idt)
          idt = this->_factory();
        return idt;
      }
      ELLE_ATTRIBUTE(Factory, factory);
      using Indentations = LocalStorage<std::unique_ptr<elle::log::Indentation>>;
      ELLE_ATTRIBUTE(Indentations, indentations);
    };

    class Indenter
      : public elle::log::Indenter
    {
    public:
      std::unique_ptr<elle::log::Indentation>
      indentation(Factory const& factory) override
      {
        return std::make_unique<Indentation>(factory);
      }
    };

    elle::Plugin<elle::log::Indenter>::Register<Indenter>
    register_indentation;

    class ThreadTag
      : public elle::log::Tag
    {
    public:
      std::string
      name() override
      {
        return "coroutine";
      }

      std::string
      content() override
      {
        static auto const max =
          elle::os::getenv("ELLE_LOG_COROUTINE_MAX_WIDTH", INT_MAX);
        if (reactor::Scheduler* sched = reactor::Scheduler::scheduler())
          if (reactor::Thread* t = sched->current())
            return t->name().substr(0, max);
        return "";
      }
    };

    elle::Plugin<elle::log::Tag>::Register<ThreadTag>
    register_tag;

    namespace plugins
    {
      elle::Plugin<elle::log::Indenter> logger_indentation(register_indentation);
      elle::Plugin<elle::log::Tag> logger_tags(register_tag);
    }
  }
}
