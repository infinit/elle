#include <boost/foreach.hpp>

#include <elle/log.hh>
#include <elle/idiom/Close.hh>

#include <reactor/thread.hh>
#include <reactor/waitable.hh>

ELLE_LOG_COMPONENT("Reactor.Thread");

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Waitable::Waitable(const std::string& name)
    : _name(name)
    , _threads()
    , _exception(0)
  {}

  Waitable::Waitable(Waitable&& source)
    : _name(source._name)
    , _threads(std::move(source._threads))
    , _exception(source._exception)
  {}

  Waitable::~Waitable()
  {
    assert(_threads.empty());
  }

  /*-------.
  | Status |
  `-------*/

  const std::string&
  Waitable::name() const
  {
    return _name;
  }

  const char*
  Waitable::type_name() const
  {
    static const char* name = "waitable";
    return name;
  }

  const Waitable::Waiters&
  Waitable::waiters() const
  {
    return _threads;
  }

  /*--------.
  | Waiting |
  `--------*/

  int
  Waitable::_signal()
  {
    if (_threads.empty())
    {
      _exception = 0;
      return false;
    }
    BOOST_FOREACH (Thread* thread, _threads)
      thread->_wake(this);
    int res = _threads.size();
    _threads.clear();
    _exception = 0;
    return res;
  }

  bool
  Waitable::_signal_one()
  {
    if (_threads.empty())
    {
      _exception = 0;
      return false;
    }
    Thread* thread = *_threads.begin();
    thread->_wake(this);
    _threads.erase(thread);
    _exception = 0;
    return true;
  }

  bool
  Waitable::_wait(Thread* t)
  {
    ELLE_TRACE("%s: wait %s", *t, *this);
    assert(_threads.find(t) == _threads.end());
    _threads.insert(t);
    return true;
  }

  void
  Waitable::_unwait(Thread* t)
  {
    ELLE_TRACE("%s: unwait %s", *t, *this);
    assert(_threads.find(t) != _threads.end());
    _threads.erase(t);
  }

  void
  Waitable::_raise(Exception* e)
  {
    _exception = e;
  }

  /*----------.
  | Waitables |
  `----------*/

  Waitables&
  operator << (Waitables& waitables, Waitable& s)
  {
    waitables.push_back(&s);
    return waitables;
  }

  Waitables&
  operator << (Waitables& waitables, Waitable* s)
  {
    waitables.push_back(s);
    return waitables;
  }

  /*-------------.
  | Pretty print |
  `-------------*/

  std::ostream& operator << (std::ostream& s, const Waitable& t)
  {
    s << t.type_name() << " ";
    if (t.name().empty())
      s << &t;
    else
      s << t.name();
    return s;
  }
};
