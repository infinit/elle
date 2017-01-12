#pragma once

#include <functional>
#include <type_traits>

#include <reactor/BackgroundOperation.hh>

namespace reactor
{
  template <typename T>
  class BackgroundFuture
  {
  public:
    using Action = std::function<T ()>;
    template<typename = std::enable_if_exists_t<decltype(T())>>
    BackgroundFuture();
    BackgroundFuture(T value);
    BackgroundFuture(Action action);
    ~BackgroundFuture();
    BackgroundFuture(BackgroundFuture<T> const& src);
    T const&
    value() const;
    T&
    value();
    BackgroundFuture<T>&
    operator =(Action action);
    BackgroundFuture<T>&
    operator =(T value);
    BackgroundFuture<T>&
    operator =(BackgroundFuture<T> const&) = delete;
    bool running() const;
    ELLE_ATTRIBUTE(boost::optional<BackgroundOperation<T>>, operation);
    ELLE_ATTRIBUTE(boost::optional<T>, value);
  private:
    void
    _resolve();
  };
}

#include <reactor/BackgroundFuture.hxx>
