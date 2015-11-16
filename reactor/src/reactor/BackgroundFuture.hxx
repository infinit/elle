#ifndef REACTOR_BACKGROUND_FUTURE_HXX
# define REACTOR_BACKGROUND_FUTURE_HXX

# include <elle/utils.hh>

# include <reactor/scheduler.hh>

namespace reactor
{
  template <typename T>
  template<
    typename std::enable_if_exists<decltype(T()), int>::type>
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
    : _operation(
      [this, action] { this->_value.emplace(action()); })
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
  {
    this->_resolve();
  }

  template <typename T>
  BackgroundFuture<T>&
  BackgroundFuture<T>::operator =(Action action)
  {
    // FIXME: could we abort the current assignment ?
    this->_resolve();
    this->_value.reset();
    this->_operation.emplace([this, action]
                             { this->_value.emplace(action()); });
    this->_operation->start();
    return *this;
  }

  template <typename T>
  BackgroundFuture<T>&
  BackgroundFuture<T>::operator =(T value)
  {
    // FIXME: could we abort the current assignment ?
    this->_resolve();
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
    elle::unconst(this)->_resolve();
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
      this->_operation.reset();
    }
    ELLE_ASSERT(!this->_operation);
    ELLE_ASSERT(this->_value);
  }
}

#endif
