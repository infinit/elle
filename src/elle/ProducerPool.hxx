

namespace elle
{
  template<typename T>
  ProducerPool<T>::ProducerPool(std::function<T()> produce,
                                int max_count,
                                int thread_count)
    : _produce(produce)
    , _max_count(max_count)
    , _terminating(false)
  {
    for (int i = 0; i < thread_count; ++i)
      this->_workers.emplace_back([this] { this->_run(); });
  }

  template<typename T>
  ProducerPool<T>::~ProducerPool()
  {
    this->_terminating = true;
    this->_producer_barrier.notify_all();
    for (int i = 0; i < signed(this->_workers.size()); ++i)
      this->_workers[i].join();
  }

  template<typename T>
  T ProducerPool<T>::get()
  {
    std::unique_lock<std::mutex> lk(this->_mutex);
    this->_consumer_barrier.wait(lk, [this]{return !this->_pool.empty(); });
    bool full = this->_pool.size() == unsigned(this->_max_count);
    auto res = std::move(this->_pool.back());
    this->_pool.pop_back();
    if (full)
      this->_producer_barrier.notify_one();
    return std::move(res);
  }

  template<typename T>
  void ProducerPool<T>::_run()
  {
    while (!this->_terminating)
    {
      auto val = this->_produce();
      std::unique_lock<std::mutex> lk(this->_mutex);
      this->_producer_barrier.wait(lk, [this] {
          return this->_pool.size() < unsigned(this->_max_count)
              || this->_terminating;
        });
      if (this->_terminating)
        return;
      this->_pool.emplace_back(std::move(val));
      if (this->_pool.size() == 1)
        this->_consumer_barrier.notify_one();
    }
  }
}
