#pragma once

#include <elle/reactor/asio.hh>

#include <elle/Error.hh>
#include <elle/IOStream.hh>
#include <elle/reactor/Barrier.hh>

namespace elle
{
  namespace reactor
  {
    /// Asynchronous stream API for reading.
    ///
    /// FDStream allows for asynchronous reads using an
    /// boost::asio::io_service.
    ///
    /// \code{.cc}
    ///
    /// void
    /// read_standard_input()
    /// {
    ///   elle::reactor::FDStream f(0);
    ///   auto json = elle::json::read(*input));
    /// }
    ///
    /// \endcode
    class FDStream
      : public elle::IOStream
    {
    public:
  #if defined(INFINIT_WINDOWS)
      using Handle = HANDLE;
  #else
      using Handle = int;
  #endif
      /// Construct using a boost::asio::io_service.
      ///
      /// \param service boost::asio::io_service to run operation in.
      /// \param fd Handle of the file descriptor.
      FDStream(boost::asio::io_service& service, Handle fd);
      /// Construct using the current Scheduler io_service.
      ///
      /// \param fd Handle of the file descriptor.
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
}
