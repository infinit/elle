#ifndef ELLE_PRODUCERPOOL_HH
# define ELLE_PRODUCERPOOL_HH

# include <condition_variable>
# include <thread>
# include <vector>

namespace elle
{

  template<typename T>
  class ProducerPool
  {
  public:
    ProducerPool(std::function<T()> produce, int max_pool_size, int thread_count = 1);
    ~ProducerPool();
    T
    get();
  private:
    void _run();
    std::function<T()> _produce;
    int _key_size;
    int _max_count;
    std::vector<T> _pool;
    std::mutex _mutex;
    std::vector<std::thread> _workers;
    std::condition_variable _producer_barrier;
    std::condition_variable _consumer_barrier;
    bool _terminating;
  };
}
#include <elle/ProducerPool.hxx>
#endif