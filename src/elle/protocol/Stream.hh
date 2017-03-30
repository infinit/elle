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
    /// An abstract stream, with two basic entry points: read and write.
    class Stream
      : public elle::Printable
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      /// Construct a Stream.
      ///
      /// @param scheduler A reactor::Scheduler.
      Stream(elle::reactor::Scheduler& scheduler);
      /// Construct a Stream.
      ///
      /// Default reactor::Scheduler is used.
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
      /// Read a buffer.
      ///
      /// @returns A Buffer.
      elle::Buffer
      read();
    protected:
      virtual
      elle::Buffer
      _read() = 0;

    /*--------.
    | Sending |
    `--------*/
    public:
      /// Write an packet.
      ///
      /// @param packet The buffer to write.
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
      /// Write an uint32_t to \a given stream.
      ///
      /// The version is for compatibility reasons. From version 0.3.0, the way
      /// the protocol serializes `int`s changed.
      ///
      /// @param s The stream to write to.
      /// @param i The int to write.
      /// @param v The version.
      static
      void
      uint32_put(std::ostream& s, uint32_t i, elle::Version const& v);
      /// Read an uint32_t from \a given stream.
      ///
      /// The version is for compatibility reasons. From version 0.3.0, the way
      /// the protocol serializes `int`s changed.
      ///
      /// @param s The stream to read from.
      /// @param v The version.
      static
      uint32_t
      uint32_get(std::istream& s, elle::Version const& v);
      /// Write an uint32_t to \a given buffer.
      ///
      /// The version is for compatibility reasons. From version 0.3.0, the way
      /// the protocol serializes `int`s changed.
      ///
      /// @param buffer The buffer to write to.
      /// @param i The int to write.
      /// @param v The version.
      static
      void
      uint32_put(elle::Buffer& buffer, uint32_t i, elle::Version const& v);
      /// Read an uint32_t from \a given stream.
      ///
      /// The version is for compatibility reasons. From version 0.3.0, the way
      /// the protocol serializes `int`s changed.
      ///
      /// @param s The stream to read from.
      /// @param v The version.
      static
      uint32_t
      uint32_get(elle::Buffer& s, elle::Version const& v);
    };
  }
}

#endif
