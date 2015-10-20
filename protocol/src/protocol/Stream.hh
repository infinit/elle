#ifndef INFINIT_PROTOCOL_STREAM_HH
# define INFINIT_PROTOCOL_STREAM_HH

# include <iosfwd>

# include <elle/Buffer.hh>
# include <elle/Printable.hh>

# include <reactor/fwd.hh>



namespace infinit
{
  namespace protocol
  {
    class Stream: public elle::Printable
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Stream(reactor::Scheduler& scheduler);
      virtual ~Stream();

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
      virtual elle::Buffer read() = 0;

    /*--------.
    | Sending |
    `--------*/
    public:
      void
      write(elle::Buffer& packet);
    protected:
      virtual
      void
      _write(elle::Buffer& packet) = 0;

    /*------------------.
    | Int serialization |
    `------------------*/
    protected:
      void _uint32_put(std::ostream& s, uint32_t  i);
      uint32_t _uint32_get(std::istream& s);
      void _uint32_put(elle::Buffer& s, uint32_t  i);
      uint32_t _uint32_get(elle::Buffer& s);
    };
  }
}

#endif
