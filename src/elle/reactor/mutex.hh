#pragma once

#include <elle/attribute.hh>
#include <elle/reactor/lockable.hh>

namespace elle
{
  namespace reactor
  {
    /// A Mutex can be acquired by a Thread, forcing following Threads to wait
    /// until it's released to acquire it.
    ///
    /// Mutex, like std::mutex, helps protecting resources from 'concurrent'
    /// access, which also happen in a co-routine environment.
    ///
    /// \code{.cc}
    ///
    /// class Socket
    /// {
    ///   ELLE_ATTRIBUTE_RX(elle::reactor::Mutex, write_mutex);
    ///   void
    ///   write(elle::ConstWeakBuffer const& buffer)
    ///   {
    ///     // Because this->_write might be implemented to write multiple
    ///     // batches of data in the socket (waiting for each batch to be
    ///     // written before writing a new one), we need to prevent multiple
    ///     // writers to access it 'simultaneously'.
    ///     elle::reactor::Lock l(this->write_mutex());
    ///     this->_write(buffer);
    ///   }
    /// };
    ///
    /// \endcode
    class Mutex
      : public Lockable
    {
    public:
      /// Construct a Mutex.
      Mutex();
      virtual
      ~Mutex();
      Mutex(Mutex&&) = default;
      /// Release the Mutex.
      ///
      /// \pre this->locked() == true.
      ///
      /// \returns false.
      bool
      release() override;
      /// Try to acquire the Mutex.
      ///
      /// \returns Whether the Mutex was locked before.
      ///
      /// \post this->locked() == true.
      bool
      acquire() override;
    protected:
      bool
      _wait(Thread* thread, Waker const& waker) override;

    private:
      /// Whether the Mutex is locked.
      ELLE_ATTRIBUTE_R(bool, locked);
    };
  }
}
