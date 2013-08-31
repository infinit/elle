#ifndef INFINIT_REACTOR_WAITABLE_HXX
# define INFINIT_REACTOR_WAITABLE_HXX

#include <exception>

namespace reactor
{
  template <typename Exception, typename... Args>
  void
  Waitable::_raise(Args&&... args)
  {
    this->_raise(std::make_exception_ptr(
                   Exception(std::forward<Args>(args)...)));
  }
}


#endif
