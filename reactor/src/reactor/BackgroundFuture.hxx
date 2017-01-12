#include <elle/utils.hh>
#include <elle/log.hh>

#include <reactor/scheduler.hh>

namespace reactor
{
  template <typename T>
  template <typename>
  BackgroundFuture<T>::BackgroundFuture()
    : _operation()
    , _value()
  {
    this->_value.emplace();
  }

  template <typename T>
  BackgroundFuture<T>::BackgroundFuture(T value)
    : _operation()
    , _value(std::move(value))
  {}

  template <typename T>
  BackgroundFuture<T>::BackgroundFuture(Action action)
    : _operation(std::move(action))
    , _value()
  {
    this->_operation->start();
  }

  template <typename T>
  BackgroundFuture<T>::BackgroundFuture(BackgroundFuture<T> const& src)
    : _operation()
    , _value(src.value())
  {}

  template <typename T>
  BackgroundFuture<T>::~BackgroundFuture()
  {}

  template <typename T>
  BackgroundFuture<T>&
  BackgroundFuture<T>::operator =(Action action)
  {
    this->_operation.reset();
    this->_value.reset();
    this->_operation.emplace(std::move(action));
    this->_operation->start();
    return *this;
  }

  template <typename T>
  BackgroundFuture<T>&
  BackgroundFuture<T>::operator =(T value)
  {
    this->_operation.reset();
    this->_value.reset();
    this->_value.emplace(std::move(value));
    return *this;
  }

  template <typename T>
  T const&
  BackgroundFuture<T>::value() const
  {
    return elle::unconst(this)->value();
  }

  template <typename T>
  T&
  BackgroundFuture<T>::value()
  {
    this->_resolve();
    return this->_value.get();
  }

  template <typename T>
  void
  BackgroundFuture<T>::_resolve()
  {
    ELLE_LOG_COMPONENT("reactor.BackgroundFuture");
    if (this->_operation)
    {
      ELLE_TRACE_SCOPE("%s: join", *this);
      if (this->_operation->running())
        reactor::wait(this->_operation.get());
      if (!this->_operation) // an other thread called _resolve and reset it
        ELLE_ASSERT(this->_value);
      else
      {
        this->_value.emplace(std::move(this->_operation->result().get()));
        this->_operation.reset();
      }
    }
    ELLE_ASSERT(!this->_operation);
    ELLE_ASSERT(this->_value);
  }

  template <typename T>
  bool
  BackgroundFuture<T>::running() const
  {
    return !this->_value;
  }
}
