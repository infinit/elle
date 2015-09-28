#ifndef REACTOR_GENERATOR_HXX
# define REACTOR_GENERATOR_HXX

namespace reactor
{
  template <typename T>
  Generator<T>::Generator(std::function<void (yielder const&)> const& driver)
    : _results()
    , _thread()
  {
    auto yield = [this] (T elt) { this->_results.put(std::move(elt)); };
    this->_thread.reset(
      new Thread("generator",
                 [this, driver, yield]
                 {
                   driver(yield);
                   this->_results.put({});
                 }));
  }

  template <typename T>
  Generator<T>::Generator(Generator<T>&& generator)
    : _results(std::move(generator._results))
    , _thread(std::move(generator._thread))
  {}

  template <typename T>
  Generator<T>::iterator::iterator()
    : _generator(nullptr)
    , _value()
    , _fetch(true)
  {}

  template <typename T>
  Generator<T>::iterator::iterator(Generator<T>& generator)
    : _generator(&generator)
    , _fetch(true)
  {}

  template <typename T>
  bool
  Generator<T>::iterator::operator !=(iterator const& other)
  {
    assert(other._generator == nullptr);
    if (this->_fetch)
    {
      this->_value = this->_generator->_results.get();
      this->_fetch = false;
    }
    return bool(this->_value);
  }

  template <typename T>
  typename Generator<T>::iterator&
  Generator<T>::iterator::operator ++()
  {
    this->_fetch = true;
    this->_value.reset();
    return *this;
  }

  template <typename T>
  T
  Generator<T>::iterator::operator *()
  {
    assert(!this->_fetch);
    return std::move(this->_value.get());
  }


  template <typename T>
  Generator<T>
  generator(
    std::function<void (typename yielder<T>::type const&)> const& driver)
  {
    return Generator<T>(driver);
  }

  template <typename T>
  typename Generator<T>::iterator
  begin(Generator<T>& g)
  {
    return typename Generator<T>::iterator(g);
  }

  template <typename T>
  typename Generator<T>::iterator
  end(Generator<T> const& g)
  {
    return typename Generator<T>::iterator();
  }
}

#endif
