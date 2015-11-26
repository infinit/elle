#ifndef REACTOR_BACKGROUND_FUTURE_HH
# define REACTOR_BACKGROUND_FUTURE_HH

# include <functional>
# include <type_traits>

# include <reactor/BackgroundOperation.hh>

namespace reactor
{
  template <typename T>
  class BackgroundFuture
  {
  public:
    typedef std::function<T ()> Action;
    template<
      typename std::enable_if_exists<decltype(T()), int>::type = 0>
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
    ELLE_ATTRIBUTE(boost::optional<BackgroundOperation<T>>, operation);
    ELLE_ATTRIBUTE(boost::optional<T>, value);
  private:
    void
    _resolve();
  };
}

# include <reactor/BackgroundFuture.hxx>

#endif
