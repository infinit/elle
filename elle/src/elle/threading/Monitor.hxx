#ifndef  ELLE_THREADING_MONITOR_HXX
# define ELLE_THREADING_MONITOR_HXX

# include <boost/thread/tss.hpp>

namespace elle
{
  namespace threading
  {
    template <typename T, typename MutexType>
    struct Monitor
    {
    public:
      typedef T             value_type;
      typedef MutexType     mutex_type;
    private:
      typedef unsigned int  counter_type;

    private:
      template <typename U>
      struct Proxy
      {
      private:
        U& _value;
        mutex_type& _mutex;
        counter_type& _counter;

      public:
        U*
        operator ->() const
        {
          return &this->_value;
        }

        Proxy(U& value, mutex_type& mutex, counter_type& counter)
          : _value(value)
          , _mutex(mutex)
          , _counter(counter)
        {
          if (++this->_counter == 1)
            this->_mutex.lock();
        }

        ~Proxy()
        {
          if (--this->_counter == 0)
            this->_mutex.unlock();
        }
      };

    private:
      T _value;
      MutexType mutable _mutex;
      boost::thread_specific_ptr<counter_type> mutable _counter;

    public:
      Monitor()
        : _value{}
        , _mutex{}
        , _counter{}
      {}

      Monitor(value_type const& value)
        : _value{value}
        , _mutex{}
        , _counter{}
      {}

      Monitor(value_type&& value)
        : _value{std::move(value)}
        , _mutex{}
        , _counter{}
      {}

      ~Monitor()
      {
        ELLE_ASSERT(_counter.get() == nullptr or *_counter == 0);
      }

      Proxy<T const>
      operator ->() const
      {
        if (this->_counter.get() == nullptr)
          this->_counter.reset(new counter_type{0});
        return Proxy<T const>{
          this->_value,
          this->_mutex,
          *this->_counter
        };
      }

      Proxy<T>
      operator ->()
      {
        if (this->_counter.get() == nullptr)
          this->_counter.reset(new counter_type{0});
        return Proxy<T>{
          this->_value,
          this->_mutex,
          *this->_counter
        };
      }

      template <typename Callable>
      decltype(std::declval<Callable>()(std::declval<T&>()))
      operator ()(Callable&& callable)
      {
        std::lock_guard<MutexType> lock{this->_mutex};
        return std::forward<Callable>(callable)(this->_value);
      }

      template <typename Callable>
      decltype(std::declval<Callable>()(std::declval<T&>()))
      operator ()(Callable&& callable) const
      {
        std::lock_guard<MutexType> lock{this->_mutex};
        return std::forward<Callable>(callable)(this->_value);
      }
    };
  }
}

#endif
