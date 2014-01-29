# ifndef INFINIT_REACTOR_CHANNEL_HH
#  define INFINIT_REACTOR_CHANNEL_HH

#include <queue>

#include <elle/attribute.hh>

#include <reactor/Barrier.hh>

namespace reactor
{

  template <class T>
  class Channel
  {

    public:
      Channel();
      void  put(T data);
      T     get();

    private:
      ELLE_ATTRIBUTE(Barrier, barrier);
      ELLE_ATTRIBUTE(std::queue<T>, queue);

  };

}

# include <reactor/Channel.hxx>

# endif /* INFINIT_REACTOR_CHANNEL_HH */
