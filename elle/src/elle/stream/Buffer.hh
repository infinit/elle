#ifndef ELLE_STREAM_BUFFER_HH
# define ELLE_STREAM_BUFFER_HH

# include <elle/OStream.hh>

namespace elle
{
  OStream
  ostream(elle::Buffer& buffer);

  namespace stream
  {
    class Buffer:
      public OStream::Backend
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      Buffer(elle::Buffer& buffer);

    /*--------.
    | Writing |
    `--------*/
    public:
      virtual
      void
      write(elle::ConstWeakBuffer const& data);
      virtual
      Size
      size();
      virtual
      Size
      size(Size size);
    private:
      ELLE_ATTRIBUTE(elle::Buffer&, buffer);
    };
  }
}

#endif
