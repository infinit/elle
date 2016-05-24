#include <elle/Plugin.hh>
#include <elle/log/Logger.hh>
#include <elle/os/environ.hh>

#include <reactor/storage.hh>

namespace reactor
{
  class Indentation:
    public elle::log::Indentation
  {
  public:
    typedef elle::log::Indenter::Factory Factory;
    Indentation(Factory const& factory)
      : _factory(factory)
      , _indentations()
    {}

    virtual
    unsigned int&
    indentation()
    {
      return this->_indentation()->indentation();
    }

    virtual
    void
    indent()
    {
      return this->_indentation()->indent();
    }

    virtual
    void
    unindent()
    {
      return this->_indentation()->unindent();
    }

    virtual
    std::unique_ptr<elle::log::Indentation>
    clone() override
    {
      auto res = elle::make_unique<Indentation>(_factory);
      return res;
    }

  private:
    std::unique_ptr<elle::log::Indentation>&
    _indentation()
    {
      std::unique_ptr<elle::log::Indentation>& idt = _indentations.Get();
      if (!idt)
        idt = this->_factory();
      return idt;
    }
    ELLE_ATTRIBUTE(Factory, factory);
    typedef LocalStorage<std::unique_ptr<elle::log::Indentation>> Indentations;
    ELLE_ATTRIBUTE(Indentations, indentations);
  };

  class Indenter:
    public elle::log::Indenter
  {
  public:
    virtual
    std::unique_ptr<elle::log::Indentation>
    indentation(Factory const& factory) override
    {
      return elle::make_unique<Indentation>(factory);
    }
  };

  elle::Plugin<elle::log::Indenter>::Register<Indenter>
  register_indentation;

  class ThreadTag:
    public elle::log::Tag
  {
  public:
    virtual
    std::string
    name()
    {
      return "coroutine";
    }

    virtual
    std::string
    content()
    {
      static std::string max_repr =
        elle::os::getenv("ELLE_LOG_COROUTINE_MAX_WIDTH", "");
      static int max = max_repr.empty() ? INT_MAX : std::atoi(max_repr.c_str());
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
