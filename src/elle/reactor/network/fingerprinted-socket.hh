#pragma once

#include <elle/reactor/network/ssl-socket.hh>

namespace elle
{
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
        FingerprintedSocket(SSLEndPoint const& endpoint,
                            std::vector<unsigned char>  fingerprint,
                            DurationOpt timeout = DurationOpt());

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
