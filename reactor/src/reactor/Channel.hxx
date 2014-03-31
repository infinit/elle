#ifndef INFINIT_REACTOR_CHANNEL_HXX
# define INFINIT_REACTOR_CHANNEL_HXX

# include <reactor/scheduler.hh>

namespace reactor
{
  template <typename T, typename Container>
  Channel<T, Container>::Channel()
    : _read_barrier(Barrier())
    , _write_barrier(Barrier())
    , _queue(Container())
    , _max_size(SizeUnlimited)
  {}

  template <typename T, typename Container>
  void
  Channel<T, Container>::put(T data)
  {
    this->_queue.push(std::move(data));
    this->_read_barrier.open();
    while (this->_queue.size() >= this->_max_size)
      reactor::wait(this->_write_barrier);
  }

  namespace details
  {
    template<typename T>
    typename T::value_type& queue_front(T& container)
    {
      return container.front();
    }
    template<typename T>
    typename std::priority_queue<T>::value_type& queue_front(std::priority_queue<T>& container)
    {
      // top() returns const& because modifying the object would break the
      // priority_queue invariants.
      // Let's hope that poping it rigth after is safe. It would make sense.
      return const_cast<typename std::priority_queue<T>::value_type&>(container.top());
    }
  }

  template <typename T, typename Container>
  T
  Channel<T, Container>::get()
  {
    /// In case of the barrier was opened
    /// with a last element, and closed immediatly
    /// Be sure the barrier is clearly opened before
    /// get element.
    while(!this->_read_barrier.opened())
      reactor::wait(this->_read_barrier);
    ELLE_ASSERT(!this->_queue.empty());
    T res(std::move(details::queue_front(this->_queue)));
    this->_queue.pop();
    if (this->_queue.empty())
      this->_read_barrier.close();
    if (this->_queue.size() < this->_max_size)
      this->_write_barrier.open();
    return res;
  }

  template <typename T, typename Container>
  unsigned int
  Channel<T, Container>::size() const
  {
    return this->_queue.size();
  }

  template <typename T, typename Container>
  void
  Channel<T, Container>::max_size(int ms)
  {
    this->_max_size = ms;
    if (this->_max_size < this->_queue.size())
      this->_write_barrier.open();
    // no need to close, next write will do that
  }

  template <typename T, typename Container>
  const T&
  Channel<T, Container>::peek()
  {
    return details::queue_front(this->_queue);
  }
}

#endif
