#ifndef INFINIT_PROTOCOL_STREAM_HH
# define INFINIT_PROTOCOL_STREAM_HH

# include <iostream>
# include <list>

# include <reactor/fwd.hh>

# include <protocol/Packet.hh>

namespace infinit
{
  namespace protocol
  {
    class Stream
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Stream(reactor::Scheduler& scheduler);

    /*-----------.
    | Properties |
    `-----------*/
    public:
      reactor::Scheduler& scheduler();
    private:
      reactor::Scheduler& _scheduler;

    /*----------.
    | Receiving |
    `----------*/
    public:
      virtual Packet read() = 0;

    /*--------.
    | Sending |
    `--------*/
    public:
      virtual void write(Packet& packet) = 0;

    /*------------------.
    | Int serialization |
    `------------------*/
    protected:
      void _uint32_put(std::ostream& s, uint32_t  i);
      uint32_t _uint32_get(std::istream& s);
    };
  }
}

#endif
