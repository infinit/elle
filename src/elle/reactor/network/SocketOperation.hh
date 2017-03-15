#pragma once

#include <boost/asio.hpp>

#include <elle/reactor/network/Error.hh>
#include <elle/reactor/network/socket.hh>
#include <elle/reactor/Operation.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// Abstract specialization of Operation for Sockets.
      ///
      /// This implements Operation::_abort.
      template <typename AsioSocket>
      class SocketOperation
        : public Operation
      {
      public:
        /// Create a SocketOperation.
        ///
        /// \param socket The Socket to perform operations.
        SocketOperation(AsioSocket& socket);
      protected:
        /// Allows for aborting Operation.
        ///
        /// \post this->canceled() == true.
        void
        _abort() override;

        void
        _wakeup(const boost::system::error_code& error);

        /// Turn ASIO connection refused related errors into ConnectionRefused
        /// exceptions and others exceptions to Exceptions.
        ///
        /// \param error The error code given by boost.
        virtual
        void
        _handle_error(boost::system::error_code const& error);

        ELLE_ATTRIBUTE_R(AsioSocket&, socket);
        ELLE_ATTRIBUTE_R(bool, canceled);
      };

      /// Abstract specialization of SocketOperation.
      ///
      /// This implements SocketOperation::_handle_error.
      template <typename AsioSocket>
      class DataOperation
        : public SocketOperation<AsioSocket>
      {
      public:
        using Super = SocketOperation<AsioSocket>;
        /// Create a DataOperation.
        ///
        /// \param socket The Socket to perform operations.
        DataOperation(AsioSocket& socket);

      protected:
        /// Turn ASIO connection closed related errors into ConnectionClosed
        /// exceptions, bad descriptor to SocketClosed and ssl short read
        /// related errors to SSLShortRead exceptions.
        /// Other errors are treated by SocketOperation::_handle_error.
        ///
        /// \param error The error code given by boost.
        void
        _handle_error(boost::system::error_code const& error) override;
      };
    }
  }
}

#ifdef INFINIT_LINUX
namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// Register a callback that will be invoked if an exception is caught
      /// in reactor_epoll_wait() from thread 'thread'.
      ///
      /// If a callback is set the exception will be intercepted, otherwise it
      /// will propagate. Call with an empty std::function to remove the
      /// callback.
      ///
      /// \param cb The Callback to be called.
      /// \param thread A pointer to the calling Thread.
      void
      epoll_interrupt_callback(std::function<void()> cb,
                               reactor::Thread* thread);
    }
  }
}

/// epoll adapters using async ios
extern "C"
{
  /// timeout is in ms.
  int
  reactor_epoll_wait(int epfd, struct epoll_event *events,
                     int maxevents, int timeout);
  /// timeout is in ms.
  int
  reactor_epoll_pwait(int epfd, struct epoll_event *events,
                      int maxevents, int timeout,
                      const sigset_t *sigmask);
}
#endif
