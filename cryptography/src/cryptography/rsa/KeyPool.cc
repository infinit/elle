#include <cryptography/rsa/KeyPool.hh>

namespace infinit
{
  namespace cryptography
  {
    namespace rsa
    {
      KeyPool::KeyPool(int key_size, int max_count, int thread_count)
        : _key_size(key_size)
        , _max_count(max_count)
        , _terminating(false)
      {
        for (int i=0; i<thread_count; ++i)
          _workers.push_back(std::thread([this] { this->_run();}));
      }

      KeyPool::~KeyPool()
      {
        _terminating = true;
        _producer_barrier.notify_all();
        for (int i=0; i<signed(_workers.size()); ++i)
          _workers[i].join();
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
          KeyPair val = keypair::generate(_key_size);
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
}
