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
        Channel();
        Channel(Self&& source);


        /*--------.
        | Content |
        `--------*/
      public:
        void
        put(T data);
        T
        get();
        const T&
        peek();
        bool
        empty() const;
        void
        clear();

        /*--------.
        | Control |
        `--------*/
      public:
        void
        open();
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
        ELLE_ATTRIBUTE(Container, queue);
        ELLE_ATTRIBUTE_R(bool, opened);
        /// Maximum size, will block writers *after* insertion if reached
        ELLE_ATTRIBUTE_Rw(int, max_size);
        ELLE_ATTRIBUTE_r(int, size);
        enum
        {
          SizeUnlimited = std::numeric_limits<int>::max()
        };
      };
  }
}

# include <elle/reactor/Channel.hxx>
