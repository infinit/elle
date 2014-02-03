#ifndef INFINIT_REACTOR_CHANNEL_HXX
# define INFINIT_REACTOR_CHANNEL_HXX

# include <reactor/scheduler.hh>

namespace reactor
{

  template <typename T>
  Channel<T>::Channel():
    _barrier(Barrier()),
    _queue(std::queue<T>())
  {}

  template <typename T>
  void
  Channel<T>::put(T data)
  {
    this->_queue.push(std::move(data));
    this->_barrier.open();
  }

  template <typename T>
  T
  Channel<T>::get()
  {
    /// In case of the barrier was opened
    /// with a last element, and closed immediatly
    /// Be sure the barrier is clearly opened before
    /// get element.
    while(!this->_barrier.opened())
      reactor::wait(this->_barrier);
    ELLE_ASSERT(!this->_queue.empty());
    T res(std::move(this->_queue.front()));
    this->_queue.pop();
    if (this->_queue.empty())
      this->_barrier.close();
    return res;
  }

}

#endif
