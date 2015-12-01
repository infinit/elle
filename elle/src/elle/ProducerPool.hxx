

namespace elle
{
  template<typename T>
  ProducerPool<T>::ProducerPool(std::function<T()> produce, int max_count, int thread_count)
  : _produce(produce)
  , _max_count(max_count)
  , _terminating(false)
  {
    for (int i=0; i<thread_count; ++i)
      _workers.push_back(std::thread([this] { this->_run();}));
  }

  template<typename T>
  ProducerPool<T>::~ProducerPool()
  {
    _terminating = true;
    _producer_barrier.notify_all();
    for (int i=0; i<signed(_workers.size()); ++i)
      _workers[i].join();
  }

  template<typename T>
  T ProducerPool<T>::get()
  {
    std::unique_lock<std::mutex> lk(_mutex);
    _consumer_barrier.wait(lk, [this]{return !_pool.empty();});
    bool full = (_pool.size() == unsigned(_max_count));
    auto res = std::move(_pool.back());
    _pool.pop_back();
    if (full)
      _producer_barrier.notify_one();
    return std::move(res);
  }

  template<typename T>
  void ProducerPool<T>::_run()
  {
    while (true)
    {
      if (_terminating)
        return;
      auto val = _produce();
      std::unique_lock<std::mutex> lk(_mutex);
      _producer_barrier.wait(lk, [this] {
          return _pool.size() < unsigned(_max_count) || _terminating;
        });
      if (_terminating)
        return;
      _pool.emplace_back(std::move(val));
      if (_pool.size() == 1)
        _consumer_barrier.notify_one();
    }
  }
}