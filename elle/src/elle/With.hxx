#ifndef ELLE_WITH_HXX
# define ELLE_WITH_HXX

# include <utility>

# include <elle/log.hh>
# include <elle/assert.hh>
# include <elle/log.hh>

namespace elle
{
  /*-------------.
  | Construction |
  `-------------*/

  template <typename T>
  template <typename ... Args>
  With<T>::With(Args&&... args):
    _used(false)
  {
    ELLE_LOG_COMPONENT("elle.With");

    ELLE_TRACE("%s: construct", *this)
      new (this->_value) T(std::forward<Args>(args)...);
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
    inline
    ReturnHolder(F const& f, V& v):
      _value(f(v))
    {}

    inline
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
    inline
    ReturnHolder(F const& f, V& v)
    {
      f(v);
    }

    inline
    void
    value()
    {}
  };

  template <typename T>
  template <typename F>
  auto
  With<T>::operator <<(F const& action) -> decltype(action(*(T*)(nullptr)))
  {
    typedef decltype(action(*(T*)(nullptr))) Value;

    ELLE_LOG_COMPONENT("elle.With");

    ELLE_ASSERT(!this->_used);
    this->_used = true;
    try
    {
      ReturnHolder<Value> res(action, reinterpret_cast<T&>(this->_value));
      ELLE_TRACE("%s: destruct", *this)
        reinterpret_cast<T&>(this->_value).~T();
      return res.value();
    }
    catch (...)
    {
      auto e = std::current_exception();
      try
      {
        ELLE_TRACE("%s: destruct", *this)
          reinterpret_cast<T&>(this->_value).~T();
      }
      catch (...)
      {
        ELLE_ERR("losing exception: %s", elle::exception_string(e));
        ELLE_ERR("overriden by: %s", elle::exception_string());
        throw;
      }
      throw;
    }
  }
}


#endif
