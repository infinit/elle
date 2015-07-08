#ifndef INFINIT_CRYPTOGRAPHY_KEYPOOL_HH
# define INFINIT_CRYPTOGRAPHY_KEYPOOL_HH

#include <condition_variable>
#include <thread>
#include <vector>

#include <cryptography/Cryptosystem.hh>
#include <cryptography/KeyPair.hh>

namespace infinit
{
  namespace cryptography
  {
    class KeyPool
    {
    public:
      KeyPool(Cryptosystem key_system, int key_size, int max_pool_size);
      ~KeyPool();
      KeyPair
      get();
    private:
      void _run();
      Cryptosystem _key_system;
      int _key_size;
      int _max_count;
      std::vector<KeyPair> _pool;
      std::mutex _mutex;
      std::thread _worker;
      std::condition_variable _producer_barrier;
      std::condition_variable _consumer_barrier;
      bool _terminating;
    };
  }
}

#endif