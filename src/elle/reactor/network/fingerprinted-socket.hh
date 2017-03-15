#pragma once

#include <elle/reactor/network/ssl-socket.hh>

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /// A specialized SSLSocket that automatically check the certificate via
      /// its fingerprint;
      class FingerprintedSocket
        : public SSLSocket
      {
      /*-------------.
      | Construction |
      `-------------*/
      public:
        /// Create a FingerprintedSocket to an SSLEndPoint.
        ///
        /// \param endpoint The SSLEndpoint to connect to.
        /// \param fingerprint The expected fingerprint of the SSL certificate.
        /// \param timeout @see SSLSocket::SSLSocket.
        FingerprintedSocket(SSLEndPoint const& endpoint,
                            std::vector<unsigned char>  fingerprint,
                            DurationOpt timeout = DurationOpt());

        /// Create a FingerprintedSocket to a host:port.
        ///
        /// \param hostname The host name to connect to.
        /// \param port The port to connect to.
        /// \param fingerprint The expected fingerprint of the SSL certificate.
        /// \param timeout @see SSLSocket::SSLSocket.
        FingerprintedSocket(const std::string& hostname,
                            const std::string& port,
                            std::vector<unsigned char> const& fingerprint,
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
}
