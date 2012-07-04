#ifndef INFINIT_PROTOCOL_PACKET_HH
# define INFINIT_PROTOCOL_PACKET_HH

# include <memory>

# include <boost/noncopyable.hpp>

# include <elle/IOStream.hh>
# include <elle/Size.hh>

namespace infinit
{
  namespace protocol
  {
    /* A protocol message.
     *
     * A chunk of binary data transmitted via the protocol.
     *
     * Packets offer the stream API. Only empty packets may be writen
     * to via the stream API, and may not be written to anymore as
     * soon as any other method has been called.
     */
    class Packet: public elle::IOStream, public boost::noncopyable
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      /** Create an empty packet.
       *
       * An empty packet can be written to via the stream API.
       */
      Packet();
      /** Move a packet.
       *
       * @param source The source packet to move data from. Becomes
       *               empty afterwards.
       */
      Packet(Packet&& source);
      /// Destroy a packet.
      ~Packet();

    /*-----------.
    | Properties |
    `-----------*/
    public:
      /// The size of the contained data.
      elle::Size size() const;

    /*--------.
    | Details |
    `--------*/
    private:
      // Streambuffer for the stream API.
      class StreamBuffer;
      StreamBuffer* _streambuffer;
      // Let the streambuffer edit _data and _data_size.
      friend class StreamBuffer;
      // Create a Packet with pre-allocated data.
      Packet(elle::Size data_size);
      // Let the packets handlers use _data directly.
      friend class Serializer;
      friend class ChanneledStream;
      // The data array.
      typedef char Byte;
      Byte* _data;
      // The size of the data array.
      elle::Size _data_size;
    };

    /*----------------.
    | Pretty printing |
    `----------------*/

    // FIXME: use a printable interface
    std::ostream& operator << (std::ostream& stream, Packet const& p);
  }
}

#endif
