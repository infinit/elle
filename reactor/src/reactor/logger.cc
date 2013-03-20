#include <elle/log/Logger.hh>

#include <reactor/storage.hh>

namespace reactor
{
  class LoggerIndentation: public elle::log::Indentation
  {
  public:
    LoggerIndentation(std::function<std::unique_ptr<Indentation> ()> const& factory)
      : _factory(factory)
      , _indentations()
    {}

    virtual
    unsigned int
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

  private:
    std::unique_ptr<Indentation>&
    _indentation()
    {
      std::unique_ptr<Indentation>& idt = _indentations.Get();
      if (!idt)
        idt = std::move(this->_factory());
      return idt;
    }
    std::function<std::unique_ptr<Indentation> ()> _factory;
    ::reactor::LocalStorage<std::unique_ptr<Indentation>> _indentations;
  };

  elle::log::RegisterIndenter<LoggerIndentation> register_indenter;

  class ThreadTag: public elle::log::Tag
  {
  public:
    virtual
    std::string
    content()
    {
      if (reactor::Scheduler* sched = reactor::Scheduler::scheduler())
        if (reactor::Thread* t = sched->current())
          return t->name();
      return "";
    }
  };

  elle::log::RegisterTag<ThreadTag> register_tag;
}
