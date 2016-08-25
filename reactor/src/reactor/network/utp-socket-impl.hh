#ifndef REACTOR_NETWORK_UTP_SOCKET_IMPL_HH
# define REACTOR_NETWORK_UTP_SOCKET_IMPL_HH

# include <reactor/network/utp-socket.hh>

# include <reactor/Barrier.hh>
# include <reactor/MultiLockBarrier.hh>
# include <reactor/mutex.hh>

namespace reactor
{
  namespace network
  {
    class UTPSocket::Impl
    {
    /*------.
    | Types |
    `------*/
    public:
      friend class UTPSocket;

    /*-------------.
    | Construction |
    `-------------*/
    public:
      Impl(UTPServer& server, utp_socket* socket, bool open);

    /*-----------.
    | Attributes |
    `-----------*/
    public:
      UTPSocket::EndPoint
      peer() const;
    private:
      ELLE_ATTRIBUTE(utp_socket*, socket);
      ELLE_ATTRIBUTE(elle::Buffer, read_buffer);
      ELLE_ATTRIBUTE(Barrier, read_barrier);
      ELLE_ATTRIBUTE(Barrier, write_barrier);
      ELLE_ATTRIBUTE(Mutex, write_mutex);
      ELLE_ATTRIBUTE(Barrier, connect_barrier);
      ELLE_ATTRIBUTE(Barrier, destroyed_barrier);
      ELLE_ATTRIBUTE_R(UTPServer&, server);
      ELLE_ATTRIBUTE(elle::ConstWeakBuffer, write);
      ELLE_ATTRIBUTE(MultiLockBarrier, pending_operations);
      ELLE_ATTRIBUTE(int, write_pos);
      ELLE_ATTRIBUTE(bool, open);
      ELLE_ATTRIBUTE(bool, closing);

    /*----------.
    | Callbacks |
    `----------*/
    public:
      void
      on_connect();
      void
      on_close();
      void
      on_read(elle::ConstWeakBuffer const&);

    /*----------.
    | Operation |
    `----------*/
    public:
      void
      _destroyed();
      void
      _write_cont();
    private:
      void
      _read();
    };

    std::ostream&
    operator <<(std::ostream& output, UTPSocket::Impl const& impl);
  }
}

#endif
