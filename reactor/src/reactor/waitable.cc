#include <elle/assert.hh>
#include <elle/container/vector.hh>
#include <elle/log.hh>
#include <elle/string/algorithm.hh>
#include <elle/Backtrace.hh>

#include <reactor/scheduler.hh>
#include <reactor/thread.hh>
#include <reactor/waitable.hh>

ELLE_LOG_COMPONENT("reactor.Thread");

namespace reactor
{
  /*-------------.
  | Construction |
  `-------------*/

  Waitable::Waitable(const std::string& name)
    : _name(name)
    , _threads()
    , _exception()
  {}

  Waitable::Waitable(Waitable&& source)
    : _name(source._name)
    , _threads(std::move(source._threads))
    , _exception(source._exception)
  {}

  Waitable::~Waitable()
  {
    this->_assert_destructible();
  }

  void
  Waitable::_assert_destructible()
  {
    if (!this->_threads.empty())
    {
      std::vector<std::string> threads;
      for (auto thread: this->_threads)
        threads.push_back(elle::sprintf("%s", *thread));
      ELLE_ABORT("%s destroyed while waited by %s",
                 *this, elle::join(threads.begin(), threads.end(), ", "));
    }
  }

  /*-------.
  | Status |
  `-------*/

  const std::string&
  Waitable::name() const
  {
    return _name;
  }

  const Waitable::Waiters&
  Waitable::waiters() const
  {
    return _threads;
  }

  /*--------.
  | Waiting |
  `--------*/

  bool
  Waitable::wait(DurationOpt timeout)
  {
    auto sched = reactor::Scheduler::scheduler();
    ELLE_ASSERT(sched);
    auto current = sched->current();
    ELLE_ASSERT(current);
    return current->wait(*this, timeout);
  }

  int
  Waitable::_signal()
  {
    if (_threads.empty())
    {
      _exception = std::exception_ptr{}; // An empty one.
      this->on_signaled()();
      return false;
    }
    for (Thread* thread: this->_threads)
      thread->_wake(this);
    int res = _threads.size();
    _threads.clear();
    _exception = std::exception_ptr{}; // An empty one.
    this->on_signaled()();
    return res;
  }

  Thread*
  Waitable::_signal_one()
  {
    if (_threads.empty())
    {
      _exception = std::exception_ptr{}; // An empty one.
      this->on_signaled()();
      return nullptr;
    }
    Thread* thread = *_threads.begin();
    thread->_wake(this);
    _threads.get<1>().erase(thread);
    _exception = std::exception_ptr{}; // An empty one.
    if (this->_threads.empty())
      this->on_signaled()();
    return thread;
  }

  bool
  Waitable::_wait(Thread* t)
  {
    ELLE_TRACE("%s: wait %s", *t, *this);
    ELLE_ASSERT_EQ(_threads.get<1>().find(t), _threads.get<1>().end());
    _threads.get<1>().insert(t);
    return true;
  }

  void
  Waitable::_unwait(Thread* t)
  {
    ELLE_TRACE("%s: unwait %s", *t, *this);
    ELLE_ASSERT_NEQ(_threads.get<1>().find(t), _threads.get<1>().end());
    _threads.get<1>().erase(t);
  }

  void
  Waitable::_raise(std::exception_ptr e)
  {
    this->_exception = e;
  }

  /*----------.
  | Printable |
  `----------*/

  void
  Waitable::print(std::ostream& stream) const
  {
    stream << elle::demangle(typeid(*this).name()) << "(" << this << ")";
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

  std::ostream&
  operator << (std::ostream& stream, Waitables const& waitables)
  {
    if (waitables.size() == 1)
      stream << **waitables.begin();
    else
    {
      stream << "{";
      bool first = true;
      for (auto const& waitable: waitables)
      {
        if (first)
          first = false;
        else
          stream << ", ";
        stream << *waitable;
      }
      stream << "}";
    }
    return stream;
  }
};
