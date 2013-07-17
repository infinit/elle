#ifndef INFINIT_REACTOR_WAITABLE_HXX
# define INFINIT_REACTOR_WAITABLE_HXX

#include <exception>

namespace reactor
{
  template <typename Exception, typename... Args>
  void
  Waitable::_raise(Args&&... args)
  {
    if (this->_exception)
    {
      try
      {
        try
        {
          std::rethrow_exception(this->_exception);
        }
        catch (...)
        {
          std::throw_with_nested(Exception{std::forward<Args>(args)...});
        }
      }
      catch (...)
      {
        this->_exception = std::current_exception();
      }
    }
    else
    {
      this->_exception = std::make_exception_ptr(Exception{
                                                   std::forward<Args>(args)...
                                                 });
    }
  }
} /* reactor */


#endif /* end of include guard: INFINIT_REACTOR_WAITABLE_HXX */
