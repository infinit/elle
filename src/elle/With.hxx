#include <utility>

#include <elle/Exception.hh>
#include <elle/assert.hh>
#include <elle/log.hh>

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  template <typename T>
  template <typename ... Args>
  With<T>::With(Args&&... args)
    : _used(false)
    , _value(reinterpret_cast<T*>(this->_data))
  {
    ELLE_LOG_COMPONENT("elle.With");
    ELLE_TRACE("%s: construct", *this)
      new (this->_value) T(std::forward<Args>(args)...);
  }

  template <typename T>
  With<T>::With(With<T>&& value)
    : _used(value._used)
    , _value(reinterpret_cast<T*>(this->_data))
  {
    ELLE_LOG_COMPONENT("elle.With");
    ELLE_TRACE("%s: construct by copy", *this)
      new (this->_value) T(std::move(*(value._value)));
  }

  template <typename T>
  With<T>::~With()
  {
    if (!this->_used)
      try
      {
        this->_value->~T();
      }
      catch (...)
      {
        ELLE_ABORT("destructor of unused elle::With threw: %s",
                   elle::exception_string());
      }
  }

  /*--------.
  | Running |
  `--------*/

  template <typename T>
  class
  ReturnHolder
  {
  public:
    template <typename F, typename V>
    ReturnHolder(F const& f, V& v)
      : _value(f(v))
    {}

    T&&
    value()
    {
      return std::move(this->_value);
    }

  private:
    T _value;
  };

  template <>
  class
  ReturnHolder<void>
  {
  public:
    template <typename F, typename V>
    ReturnHolder(F const& f, V& v)
    {
      f(v);
    }

    void
    value()
    {}
  };

  template <typename T>
  template <typename F>
  auto
  With<T>::operator <<(F const& action) -> decltype(action())
  {
    return this->_run([&] (T&) { return action(); });
  }

  template <typename T>
  template <typename F>
  auto
  With<T>::operator <<(F const& action) -> decltype(action(std::declval<T&>()))
  {
    return this->_run(action);
  }

  template <typename T>
  ELLE_COMPILER_ATTRIBUTE_NO_INLINE
  void
  With<T>::destroy_it(T* ptr)
  {
    ELLE_LOG_COMPONENT("elle.With");
    ELLE_TRACE("destroying value");
    ptr->~T();
    ELLE_TRACE("value destroyed");
  }

// Compiling with mingw gcc 4.8 and 5.3 with -O2 chokes on the traces:
// it calls log::Send's dtor with this = nullptr.
#ifdef INFINIT_WINDOWS
# define ELLE_TRACE_ ELLE_TRACE
#else
# define ELLE_TRACE_(...) {}
#endif

  template <typename T>
  template <typename F>
  auto
  With<T>::_run(F const& action) -> decltype(action(std::declval<T&>()))
  {
    ELLE_LOG_COMPONENT("elle.With");
    ELLE_ASSERT(!this->_used);

    this->_used = true;
    bool succeeded = false;
    try
    {
      using Value = decltype(action(std::declval<T&>()));
      auto res = ReturnHolder<Value>(action, *this->_value);
      succeeded = true;
      ELLE_TRACE_("%s: destruct", this)
      this->destroy_it(this->_value);
      ELLE_TRACE_("%s: return result", this);
      return res.value();
    }
    catch (...)
    {
      ELLE_TRACE_("%s: caught exception with succeeded=%s: %s",
                  this, succeeded, elle::exception_string());
      auto e = std::current_exception();
      if (!succeeded)
      {
        try
        {
          ELLE_TRACE_("%s: destruct", this)
          this->destroy_it(this->_value);
        }
        catch (...)
        {
          ELLE_ERR("losing exception: %s", elle::exception_string(e));
          e = std::current_exception();
          ELLE_ERR("overriden by: %s", elle::exception_string(e));
        }
      }
      ELLE_TRACE_("%s: rethrowing exception: %s",
                  this, elle::exception_string(e));
      std::rethrow_exception(e);
    }
    elle::unreachable();
  }
#undef ELLE_TRACE_
}
