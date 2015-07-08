#include <cryptography/KeyPool.hh>

namespace infinit
{
  namespace cryptography
  {
    KeyPool::KeyPool(Cryptosystem key_system, int key_size, int max_count)
    : _key_system(key_system)
    , _key_size(key_size)
    , _max_count(max_count)
    , _terminating(false)
    {
      _worker = std::thread([this] { this->_run();});
    }
    KeyPool::~KeyPool()
    {
      _terminating = true;
      _producer_barrier.notify_one();
      _worker.join();
    }
    KeyPair KeyPool::get()
    {
      std::unique_lock<std::mutex> lk(_mutex);
      _consumer_barrier.wait(lk, [this]{return !_pool.empty();});
      bool full = (_pool.size() == unsigned(_max_count));
      KeyPair res = std::move(_pool.back());
      _pool.pop_back();
      if (full)
        _producer_barrier.notify_one();
      return std::move(res);
    }

    void KeyPool::_run()
    {
      while (true)
      {
        if (_terminating)
          return;
        KeyPair val = KeyPair::generate(_key_system, _key_size);
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
}