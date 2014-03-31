# ifndef INFINIT_REACTOR_CHANNEL_HH
#  define INFINIT_REACTOR_CHANNEL_HH

#include <queue>
#include <limits>

#include <elle/attribute.hh>

#include <reactor/Barrier.hh>

namespace reactor
{
  template <class T, class Container = std::queue<T>>
  class Channel
  {
  public:
    Channel();
    void
    put(T data);
    T
    get();
    const T&
    peek();
    unsigned int
    size() const;

  private:
    ELLE_ATTRIBUTE(Barrier, read_barrier);
    ELLE_ATTRIBUTE(Barrier, write_barrier);
    ELLE_ATTRIBUTE(Container, queue);
    /// Maximum size, will block writers *after* insertion if reached
    ELLE_ATTRIBUTE_Rw(int, max_size);
    enum
    {
      SizeUnlimited = std::numeric_limits<int>::max()
    };
  };
}

# include <reactor/Channel.hxx>

# endif
