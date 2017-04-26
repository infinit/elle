#pragma once

#include <queue>
#include <limits>

#include <elle/Printable.hh>
#include <elle/attribute.hh>
#include <elle/reactor/Barrier.hh>

namespace elle
{
  namespace reactor
  {
    /// Channel is a container where you can put and get data. Trying to put
    /// data in a Channel already full will force the Thread to wait. Trying
    /// to get data from an empty Channel will block Thread.
    ///
    /// @code{.cc}
    ///
    /// Channel<int> c;
    /// c.max_size(2);
    /// elle::With<elle::reactor::Scope>() << [&](elle::reactor::Scope &s)
    /// {
    ///   elle::reactor::Barrier production_over;
    ///   s.run_background("producer",
    ///                    [&]
    ///                    {
    ///                      for (auto i = 0u; i < 5u; ++i)
    ///                      {
    ///                        std::cout << "put 42.";
    ///                        channel.put(i);
    ///                      }
    ///                      production_over.open();
    ///                    });
    ///   s.run_background("consumer",
    ///                    [&]
    ///                    {
    ///                      while (!production_over.opened() &&
    ///                             !channel.empty())
    ///                      {
    ///                        auto res = channel.get();
    ///                        std::cout << "get " << res << ".";
    ///                      }
    ///                    });
    ///   s.wait();
    /// };
    /// // Result:
    /// // put 0.put 1.get 0.get 1.put 2.put 3.get 3.get 4.put 4.get 4.
    template <class T, class Container = std::queue<T>>
    class Channel
      : public elle::Printable
    {
      /*------.
      | Types |
      `------*/
    public:
      using Self = Channel<T, Container>;

      /*-------------.
      | Construction |
      `-------------*/
    public:
      /// Create a Channel.
      Channel();
      /// Create a Channel from another Channel. Data are acquired by this
      /// Channel.
      Channel(Self&& source);

      /*--------.
      | Content |
      `--------*/
    public:
      /// Put data.
      ///
      /// If _size >= _max_size, yields until _size < _max_size.
      ///
      /// @tparam T The type of data stored by the Channel.
      /// @param data The data to store.
      ///
      /// @post _read_barrier is opened if opened == true.
      /// @post _size <= _max_size.
      void
      put(T data);
      /// Get data and pop it from the Channel.
      ///
      /// If _read_barrier is not opened, wait until it is.
      ///
      /// @tparam T Type stored by the Channel.
      /// @returns Queued data.
      ///
      /// @post _write_barrier.
      /// @post _read_barrier is closed if _size == 0.
      T
      get();
      /// Get data from the Channel without altering it.
      ///
      /// If _read_barrier is not opened, wait until it is.
      ///
      /// @tparam T Type stored by the Channel.
      /// @returns Queued data.
      const T&
      peek() const;
      /// Whether the Channel is empty.
      bool
      empty() const;
      /// Empty the Channel.
      void
      clear();
      /// Raise an exception when empty and getting an element.
      void
      raise(std::exception_ptr e);
      /// Raise an exception when empty and getting an element.
      template <typename E = elle::Error, typename ... Args>
      void
      raise(Args&& ... args);
    private:
      void
      _exhausted();

    /*--------.
    | Control |
    `--------*/
    public:
      /// Open the Channel for reading.
      void
      open();
      /// Close the Channel for reading.
      void
      close();

      /*----------.
      | Printable |
      `----------*/
    protected:
      void
      print(std::ostream& stream) const override;

    private:
      ELLE_ATTRIBUTE(Barrier, read_barrier);
      ELLE_ATTRIBUTE(Barrier, write_barrier);
      ELLE_ATTRIBUTE(std::exception_ptr, exception);
      ELLE_ATTRIBUTE(Container, queue);
      ELLE_ATTRIBUTE_R(bool, opened);
      /// Maximum size, will block writers *after* insertion if reached
      ELLE_ATTRIBUTE_Rw(int, max_size);
      ELLE_ATTRIBUTE_r(int, size);
      ELLE_ATTRIBUTE_RX(boost::signals2::signal<void ()>, on_get);
      ELLE_ATTRIBUTE_RX(boost::signals2::signal<void ()>, on_put);
      enum
      {
        SizeUnlimited = std::numeric_limits<int>::max()
      };
    };
  }
}

# include <elle/reactor/Channel.hxx>
