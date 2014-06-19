# ifndef INFINIT_REACTOR_CHANNEL_HH
#  define INFINIT_REACTOR_CHANNEL_HH

#include <queue>
#include <limits>

#include <elle/attribute.hh>
#include <elle/Printable.hh>

#include <reactor/Barrier.hh>

namespace reactor
{
  template <class T, class Container = std::queue<T>>
  class Channel
    : public elle::Printable
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
    bool
    empty() const;
    void
    clear();
    void
    open();
    void
    close();

  /*----------.
  | Printable |
  `----------*/
  protected:
    virtual
    void
    print(std::ostream& stream) const override;

  private:
    ELLE_ATTRIBUTE(Barrier, read_barrier);
    ELLE_ATTRIBUTE(Barrier, write_barrier);
    ELLE_ATTRIBUTE(Container, queue);
    ELLE_ATTRIBUTE_R(bool, opened);
    /// Maximum size, will block writers *after* insertion if reached
    ELLE_ATTRIBUTE_Rw(unsigned int, max_size);
    enum
    {
      SizeUnlimited = std::numeric_limits<int>::max()
    };
  };
}

# include <reactor/Channel.hxx>

# endif
