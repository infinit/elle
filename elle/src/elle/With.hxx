#ifndef ELLE_WITH_HXX
# define ELLE_WITH_HXX

# include <utility>

# include <elle/assert.hh>
# include <elle/log.hh>

namespace elle
{
  template <typename T>
  template <typename ... Args>
  With<T>::With(Args&&... args):
    _used(false)
  {
    new (this->_value) T(std::forward<Args>(args)...);
  }

  template <typename T>
  template <typename F>
  void
  With<T>::operator <<(F const& action)
  {
    ELLE_LOG_COMPONENT("elle.With");

    ELLE_ASSERT(!this->_used);
    this->_used = true;
    try
    {
      action(reinterpret_cast<T&>(this->_value));
    }
    catch (...)
    {
      auto e = std::current_exception();
      try
      {
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
    reinterpret_cast<T&>(this->_value).~T();
  }
}


#endif
