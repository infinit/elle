#pragma once

#include <elle/Error.hh>
#include <elle/optional.hh>
#include <elle/reactor/Channel.hh>
#include <elle/reactor/Thread.hh>

namespace elle
{
  namespace reactor
  {
    template <typename T>
    struct yielder
    {
      using type = std::function<void (T)>;
    };

    /// Creates an iterable object over a function. This is powerful in
    /// asynchronous environment when need to wait for at least n values
    /// (see example).
    ///
    /// Results being put in a Channel, you can get all the benefits of the
    /// Channel (mostly waiting for result).
    ///
    /// \code{.cc}
    ///
    /// auto f = [&] (elle::reactor::yielder<int>::type const& yield)
    ///   {
    ///     elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
    ///     {
    ///       // Lets consider sources, a list of sources you want to check to
    ///       // make sure some information is relevant.
    ///       for (auto const& source: sources)
    ///       {
    ///         s.run_background([&source]
    ///                          {
    ///                            reactor::http::Request r(source);
    ///                            r.finalize();
    ///                            if (r.ok())
    ///                              yield(source);
    ///                          });
    ///       }
    ///       scope.wait();
    ///     };
    ///   };
    /// for (auto const& response: elle::reactor::generator(f))
    ///   std::cout << response;
    /// // Result: Only the valid sources.
    ///
    /// \endcode
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
      using yielder = typename yielder<T>::type;
      using Driver = std::function<void (yielder const&)>;
      /// Create a generator on a driver.
      Generator(Driver const& driver);
      Generator(Generator&&b);
      ~Generator();

    /*--------.
    | Content |
    `--------*/
    public:
      /// Get the next result element of the Generator.
      ///
      /// If it's not yet available, wait until it is.
      /// If there is not more element, throw a End exception.
      ///
      /// \returns The next element.
      T
      next();

    /*---------.
    | Iterator |
    `---------*/
    public:
      /// Iterator used by the Generator.
      struct iterator
      {
        iterator();
        iterator(Generator<T>& generator);
        /// Compare iterator.
        bool
        operator !=(iterator const& other);
        /// Return the successor of the iterator.
        iterator&
        operator ++();
        /// Return the element previous to current.
        T
        operator *();
        ELLE_ATTRIBUTE(Generator<T>*, generator)
        ELLE_ATTRIBUTE(boost::optional<T>, value);
        ELLE_ATTRIBUTE(bool, fetch);
      };
      /// Return an iterator to the beginning of the Generator.
      iterator
      begin();
      /// Return an iterator to the end of the Generator.
      iterator
      end() const;
      ELLE_ATTRIBUTE(reactor::Channel<boost::optional<T>>, results);
      ELLE_ATTRIBUTE(std::exception_ptr, exception);
      ELLE_ATTRIBUTE(reactor::Thread::unique_ptr, thread);
    };

    /// Construct a generator from a Driver.
    template <typename T>
    Generator<T>
    generator(
      std::function<void (typename yielder<T>::type const&)> const& driver);

    /// Return an iterator to the beginning of the given Generator.
    template <typename T>
    typename Generator<T>::iterator
    begin(Generator<T>& g);

    /// Return an iterator to the end of the given Generator.
    template <typename T>
    typename Generator<T>::iterator
    end(Generator<T> const& g);
  }
}

# include <elle/reactor/Generator.hxx>
