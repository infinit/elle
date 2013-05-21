#ifndef INFINIT_REACTOR_WAITABLE_HXX
# define INFINIT_REACTOR_WAITABLE_HXX

namespace reactor
{
  template <typename Exception, typename... Args>
  void
  Waitable::_raise(Args&&... args)
  {
    auto e = Exception{std::forward<Args>(args)...};
    this->_exception = std::make_exception_ptr(std::move(e));
  }
} /* reactor */


#endif /* end of include guard: INFINIT_REACTOR_WAITABLE_HXX */
