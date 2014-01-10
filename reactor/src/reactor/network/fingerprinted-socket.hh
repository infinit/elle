#ifndef INFINIT_REACTOR_NETWORK_FINGERPRINTED_SOCKET_HH
# define INFINIT_REACTOR_NETWORK_FINGERPRINTED_SOCKET_HH

# include <reactor/network/ssl-socket.hh>

namespace reactor
{
  namespace network
  {
    class FingerprintedSocket:
      public SSLSocket
    {
    /*-------------.
    | Construction |
    `-------------*/
    public:
      FingerprintedSocket(reactor::Scheduler& sched,
                          SSLEndPoint const& endpoint,
                          SSLCertificate const& certificate,
                          std::vector<unsigned char> const& fingerprint,
                          Handshake_type type = Handshake_type::client,
                          DurationOpt timeout = DurationOpt());

      FingerprintedSocket(SSLEndPoint const& endpoint,
                          SSLCertificate const& certificate,
                          std::vector<unsigned char> const& fingerprint,
                          DurationOpt timeout = DurationOpt(),
                          Handshake_type type = Handshake_type::client);

      FingerprintedSocket(const std::string& hostname,
                          const std::string& port,
                          SSLCertificate const& certificate,
                          std::vector<unsigned char> const& fingerprint,
                          Handshake_type type = Handshake_type::client,
                          DurationOpt timeout = DurationOpt());

      ~FingerprintedSocket();

    private:
      void
      _check_certificate();

    private:
      ELLE_ATTRIBUTE(std::vector<unsigned char>, fingerprint);
    };

  }
}

#endif // INFINIT_REACTOR_NETWORK_FINGERPRINTED_SOCKET_HH
