#include <type_traits>

namespace elle
{
  namespace reactor
  {
    /*----.
    | End |
    `----*/

    template <typename T>
    Generator<T>::End::End(Generator<T> const& g)
      : elle::Error(elle::sprintf("%s exhausted", g))
    {}

    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    template <typename Driver>
    Generator<T>::Generator(Driver driver)
    {
      using Signature = std::function<auto (yielder const&) -> void>;
      static_assert(std::is_constructible<Signature, Driver>::value, "");
      ELLE_LOG_COMPONENT("elle.reactor.Generator");
      auto yield = [this] (T elt) { this->_results.put(std::move(elt)); };
      this->_thread.reset(
        new Thread("generator",
                   [this, driver, yield]
                   {
                     try
                     {
                       driver(yield);
                     }
                     catch (...)
                     {
                       ELLE_TRACE("%s: handle exception: %s",
                                  this, elle::exception_string());
                       this->_exception = std::current_exception();
                     }
                     this->_results.put({});
                   }));
    }

    template <typename T>
    Generator<T>::Generator(Generator<T>&& generator)
      : _results(std::move(generator._results))
      , _thread(std::move(generator._thread))
    {}

    template <typename T>
    Generator<T>::~Generator()
    {
      ELLE_LOG_COMPONENT("elle.reactor.Generator");
      ELLE_TRACE("%s: destruct", this);
    }


    /*--------.
    | Content |
    `--------*/

    template <typename T>
    T
    Generator<T>::iterator::operator *() const
    {
      assert(!this->_fetch);
      return std::move(this->_value.get());
    }

    /*---------.
    | Iterator |
    `---------*/

    template <typename T>
    Generator<T>::iterator::iterator()
      : _generator(nullptr)
      , _fetch(true)
    {}

    template <typename T>
    Generator<T>::iterator::iterator(Generator<T>& generator)
      : _generator(&generator)
      , _fetch(true)
    {}

    template <typename T>
    bool
    Generator<T>::iterator::operator !=(iterator const& other) const
    {
      assert(other._generator == nullptr);
      if (this->_fetch)
      {
        this->_value = this->_generator->_results.get();
        this->_fetch = false;
      }
      if (this->_value)
        return true;
      else if (this->_generator->_exception)
        std::rethrow_exception(this->_generator->_exception);
      else
        return false;
    }

    template <typename T>
    bool
    Generator<T>::iterator::operator ==(iterator const& other) const
    {
      return !(*this != other);
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
    void
    Generator<T>::iterator::operator ++(int)
    {
      ++*this;
    }

    template <typename T>
    T
    Generator<T>::next()
    {
      if (auto res = this->_results.get())
        return *res;
      else
        throw End(*this);
    }


    template <typename T>
    Generator<T>
    generator(std::function<void (yielder<T> const&)> const& driver)
    {
      return Generator<T>(driver);
    }

    template <typename T>
    typename Generator<T>::iterator
    Generator<T>::begin()
    {
      return typename Generator<T>::iterator(*this);
    }

    template <typename T>
    typename Generator<T>::iterator
    Generator<T>::end()
    {
      return typename Generator<T>::iterator();
    }

    template <typename T>
    typename Generator<T>::const_iterator
    Generator<T>::begin() const
    {
      return typename Generator<T>::iterator(elle::unconst(*this));
    }

    template <typename T>
    typename Generator<T>::const_iterator
    Generator<T>::end() const
    {
      return typename Generator<T>::iterator();
    }
  }
}
