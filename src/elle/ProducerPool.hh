#ifndef ELLE_PRODUCERPOOL_HH
# define ELLE_PRODUCERPOOL_HH

# include <condition_variable>
# include <thread>
# include <vector>

namespace elle
{
  /// A production pool using std::thread to generate instance of a given type.
  ///
  /// \code{.cc}
  ///
  /// Production p([] { return std::this_thread::sleep_for(2_sec); return 3; },
  ///              30,
  ///              4);
  /// p.get();
  ///
  /// \endcode
  template<typename T>
  class ProducerPool
  {
  public:
    /// Create a ProducerPool.
    ///
    /// @param produce The production function.
    /// @param max_pool_size The maximum size of the pool.
    /// @param thread_count The number of parallel std::threads producing
    ///                     values.
    ProducerPool(std::function<T()> produce,
                 int max_pool_size,
                 int thread_count = 1);
    ~ProducerPool();
    /// Get a value from the pool.
    ///
    /// @returns A value from the pool.
    T
    get();
  private:
    void
    _run();

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

# include <elle/ProducerPool.hxx>

#endif
