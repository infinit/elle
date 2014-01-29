#ifndef INFINIT_REACTOR_CHANNEL_HXX
# define INFINIT_REACTOR_CHANNEL_HXX

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
    this->_queue.push(data);
    this->_barrier.open();
  }

  template <typename T>
  T
  Channel<T>::get()
  {
    wait(this->_barrier);
    T res = this->_queue.front();
    this->_queue.pop();
    if (this->_queue.empty())
      this->_barrier.close();
    return res;
  }

}

#endif
