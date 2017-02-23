#pragma once

#include <elle/Error.hh>
#include <elle/optional.hh>
#include <elle/reactor/Channel.hh>
#include <elle/reactor/thread.hh>

namespace elle
{
  namespace reactor
  {
    template <typename T>
    struct yielder
    {
      typedef std::function<void (T elt)> type;
    };

    template <typename T>
    struct Generator
    {
    /*----.
    | End |
    `----*/
    public:
      class End
        : public elle::Error
      {
      public:
        End(Generator const& g);
      };

    /*-------------.
    | Construction |
    `-------------*/
    public:
      typedef typename yielder<T>::type yielder;
      Generator(std::function<void (yielder const&)> const& driver);
      Generator(Generator&&b);
      ~Generator();

    /*--------.
    | Content |
    `--------*/
    public:
      T
      next();

    /*---------.
    | Iterator |
    `---------*/
    public:
      struct iterator
      {
        iterator();
        iterator(Generator<T>& generator);
        bool
        operator !=(iterator const& other);
        iterator&
        operator ++();
        T
        operator *();
        ELLE_ATTRIBUTE(Generator<T>*, generator)
        ELLE_ATTRIBUTE(boost::optional<T>, value);
        ELLE_ATTRIBUTE(bool, fetch);
      };
      iterator
      begin();
      iterator
      end() const;
      ELLE_ATTRIBUTE(reactor::Channel<boost::optional<T>>, results);
      ELLE_ATTRIBUTE(std::exception_ptr, exception);
      ELLE_ATTRIBUTE(reactor::Thread::unique_ptr, thread);
    };

    template <typename T>
    Generator<T>
    generator(
      std::function<void (typename yielder<T>::type const&)> const& driver);

    template <typename T>
    typename Generator<T>::iterator
    begin(Generator<T>& g);

    template <typename T>
    typename Generator<T>::iterator
    end(Generator<T> const& g);
  }
}

# include <elle/reactor/Generator.hxx>
