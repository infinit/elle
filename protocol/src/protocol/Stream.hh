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
      Stream();
      virtual
      ~Stream();

    /*-----------.
    | Properties |
    `-----------*/
      ELLE_ATTRIBUTE_R(reactor::Scheduler&, scheduler);

    /*----------.
    | Receiving |
    `----------*/
    public:
      virtual
      elle::Buffer
      read() = 0;

    /*--------.
    | Sending |
    `--------*/
    public:
      void
      write(elle::Buffer const& packet);
    protected:
      virtual
      void
      _write(elle::Buffer const& packet) = 0;

    /*------------------.
    | Int serialization |
    `------------------*/
    public:
      static
      void
      uint32_put(std::ostream& s, uint32_t i, elle::Version const& v);

      static
      uint32_t
      uint32_get(std::istream& s, elle::Version const& v);

      static
      void
      uint32_put(elle::Buffer& s, uint32_t  i, elle::Version const& v);

      static
      uint32_t
      uint32_get(elle::Buffer& s, elle::Version const& v);
    };
  }
}

#endif
