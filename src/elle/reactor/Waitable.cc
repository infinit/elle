#include <elle/Backtrace.hh>
#include <elle/assert.hh>
#include <elle/log.hh>
#include <elle/make-vector.hh>
#include <elle/reactor/scheduler.hh>
#include <elle/reactor/Thread.hh>
#include <elle/reactor/Waitable.hh>
#include <elle/string/algorithm.hh>
#include <utility>

ELLE_LOG_COMPONENT("elle.reactor.Thread");

namespace elle
{
  namespace reactor
  {
    /*-------------.
    | Construction |
    `-------------*/

    Waitable::Waitable(std::string name)
      : _name(std::move(name))
      , _waiters()
      , _exception()
    {}

    Waitable::Waitable(Waitable&& source)
      : _name(source._name)
      , _waiters(std::move(source._waiters))
      , _exception(source._exception)
    {}

    Waitable::~Waitable()
    {
      this->_assert_destructible();
    }

    void
    Waitable::_assert_destructible()
    {
      if (!this->_waiters.empty())
      {
        auto threads =
          make_vector(this->_waiters,
                      [](auto& t){ return elle::sprintf("%s", *t.first); });
        ELLE_ABORT("%s destroyed while waited by %s at %s",
                   *this,
                   elle::join(threads.begin(), threads.end(), ", "),
                   elle::Backtrace::current());
      }
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
      if (_waiters.empty())
      {
        _exception = std::exception_ptr{}; // An empty one.
        this->on_signaled()();
        return false;
      }
      for (auto& thread: this->_waiters)
        if (thread.second)
          thread.second(thread.first);
        else
          thread.first->_wake(this);
      int res = _waiters.size();
      _waiters.clear();
      _exception = std::exception_ptr{}; // An empty one.
      this->on_signaled()();
      return res;
    }

    Thread*
    Waitable::_signal_one()
    {
      if (this->_waiters.empty())
      {
        this->_exception = std::exception_ptr{}; // An empty one.
        this->on_signaled()();
        return nullptr;
      }
      auto thread = *this->_waiters.begin();
      this->_signal_one(thread);
      return thread.first;
    }

    void
    Waitable::_signal_one(Thread* t)
    {
      auto thread = this->_waiters.get<1>().find(t);
      if (thread != this->_waiters.get<1>().end())
        return _signal_one(*thread);
    }

    void
    Waitable::_signal_one(Handler const& thread)
    {
      if (thread.second)
        thread.second(thread.first);
      else
        thread.first->_wake(this);
      this->_waiters.get<1>().erase(thread.first);
      this->_exception = std::exception_ptr{}; // An empty one.
      if (this->_waiters.empty())
        this->on_signaled()();
    }

    bool
    Waitable::_wait(Thread* t, Waker const& waker)
    {
      ELLE_TRACE("%s: wait %s", t, this);
      ELLE_ASSERT_EQ(this->_waiters.get<1>().find(t),
                     this->_waiters.get<1>().end());
      this->_waiters.emplace_back(t, waker);
      return true;
    }

    void
    Waitable::_unwait(Thread* t)
    {
      ELLE_TRACE("%s: unwait %s", t, this);
      ELLE_ASSERT_NEQ(this->_waiters.get<1>().find(t),
                      this->_waiters.get<1>().end());
      this->_waiters.get<1>().erase(t);
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
      stream << elle::type_info(*this) << "(";
      if (this->_name.empty())
        stream << this;
      else
        stream << this->_name;
      stream << ")";
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
  }
}
