#ifndef REACTOR_FD_STREAM_HH
# define REACTOR_FD_STREAM_HH

# include <boost/asio.hpp>

# include <elle/Error.hh>
# include <elle/IOStream.hh>

# include <reactor/Barrier.hh>

namespace reactor
{
  class FDStream
    : public elle::IOStream
  {
  public:
#if defined(INFINIT_WINDOWS)
    typedef HANDLE Handle;
#else
    typedef int Handle;
#endif
    FDStream(boost::asio::io_service& service, Handle fd);
    FDStream(Handle fd);

    class StreamBuffer
      : public elle::PlainStreamBuffer
    {
    public:
      StreamBuffer(boost::asio::io_service& service, Handle fd);
      virtual
      Size
      read(char* buffer, Size size) override;
      virtual
      void
      write(char* buffer, Size size) override;
#if defined(INFINIT_WINDOWS)
      ELLE_ATTRIBUTE(boost::asio::windows::stream_handle, stream);
#else
      ELLE_ATTRIBUTE(boost::asio::posix::stream_descriptor, stream);
#endif
      ELLE_ATTRIBUTE_R(Handle, handle);
    };
  };
}

#endif
