#ifndef ELLE_PROTOCOL_STREAM_HH
# define ELLE_PROTOCOL_STREAM_HH

# include <iosfwd>

# include <elle/Buffer.hh>
# include <elle/Printable.hh>
# include <elle/Version.hh>

# include <elle/reactor/fwd.hh>

namespace elle
{
  namespace protocol
  {
    class Stream: public elle::Printable
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Stream(elle::reactor::Scheduler& scheduler);
      Stream();
      virtual
      ~Stream();

    /*-----------.
    | Properties |
    `-----------*/
      ELLE_ATTRIBUTE_R(elle::reactor::Scheduler&, scheduler);
      /// The protocol is versioned.
      ///
      /// Expose the version of the backend.
      ELLE_attribute_r(elle::Version, version, virtual) = 0;

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
      uint32_put(elle::Buffer& s, uint32_t i, elle::Version const& v);

      static
      uint32_t
      uint32_get(elle::Buffer& s, elle::Version const& v);
    };
  }
}

#endif
