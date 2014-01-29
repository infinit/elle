# ifndef INFINIT_REACTOR_CHANNEL_HH
#  define INFINIT_REACTOR_CHANNEL_HH

#include <queue>

#include <elle/attribute.hh>

#include <reactor/Barrier.hh>



namespace reactor
{
  template <class T>
  class Channel:
    public std::queue<T>
  {

    public:
      Channel();

    private:
      ELLE_ATTRIBUTE(Barrier, barrier);
      ELLE_ATTRIBUTE(std::queue<T>, queue);

    public:

      void  put(T data);
      T     get();


  };

}

# include <reactor/Channel.hxx>

# endif /* INFINIT_REACTOR_CHANNEL_HH */
