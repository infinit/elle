#ifndef ELLE_BARRIER_HXX
# define ELLE_BARRIER_HXX

namespace reactor
{
  template <typename E, typename ... Args>
  void
  Barrier::raise(Args&& ... args)
  {
    this->_raise<E>(std::forward(args)...);
    this->open();
    this->_raise<E>(std::forward(args)...);
  }

}

#endif
