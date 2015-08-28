#ifndef REACTOR_FD_STREAM_HH
# define REACTOR_FD_STREAM_HH

# include <boost/asio.hpp>

# include <elle/Error.hh>
# include <elle/IOStream.hh>

# include <reactor/Barrier.hh>

#ifdef EOF
# undef EOF
#endif

namespace reactor
{
  class FDStream
    : public elle::IOStream
  {
  public:
    FDStream(boost::asio::io_service& service, int fd);
    FDStream(int fd);

    class EOF
      : public elle::Error
    {
    public:
      EOF();
    };

    class StreamBuffer
      : public elle::PlainStreamBuffer
    {
    public:
      StreamBuffer(boost::asio::io_service& service, int fd);
      virtual
      Size
      read(char* buffer, Size size) override;
      virtual
      void
      write(char* buffer, Size size) override;
      ELLE_ATTRIBUTE(boost::asio::posix::stream_descriptor, stream);
      ELLE_ATTRIBUTE_R(int, fd);
    };
  };
}

#endif
