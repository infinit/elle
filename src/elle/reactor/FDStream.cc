#include <elle/assert.hh>
#include <elle/Buffer.hh>
#include <elle/reactor/FDStream.hh>
#include <elle/reactor/scheduler.hh>

namespace elle
{
  namespace reactor
  {
    FDStream::StreamBuffer::StreamBuffer(boost::asio::io_service& service,
                                         Handle handle)
      : _stream(service, handle)
      , _handle(handle)
    {}

    elle::PlainStreamBuffer::Size
    FDStream::StreamBuffer::read(char* buffer, elle::PlainStreamBuffer::Size size)
    {
      Buffer::Size read = 0;
      boost::system::error_code error;
      reactor::Barrier done("read done");
      this->_stream.async_read_some(
        boost::asio::buffer(buffer, size),
        [&] (boost::system::error_code const& e, std::size_t s)
        {
          if (e && e == boost::asio::error::operation_aborted)
            return;
          error = e;
          read = s;
          done.open();
        });
      reactor::wait(done);
      if (error && error != boost::asio::error::eof)
        throw elle::Error(
          elle::sprintf("unable to read from %s: %s",
                        this->_handle, error.message()));
      else
        return read;
    }

    void
    FDStream::StreamBuffer::write(char* buffer, Size size)
    {
      elle::unreachable();
    }

    FDStream::FDStream(boost::asio::io_service& service, Handle handle)
      : elle::IOStream(new StreamBuffer(service, handle))
    {}

    FDStream::FDStream(Handle handle)
      : FDStream(reactor::scheduler().io_service(), handle)
    {}
  }
}
