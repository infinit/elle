#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/evp.h>

#include <elle/reactor/network/fingerprinted-socket.hh>
#include <elle/reactor/network/exception.hh>
#include <elle/reactor/scheduler.hh>
#include <utility>

ELLE_LOG_COMPONENT("elle.reactor.network.FingerprintedSocket");

namespace elle
{
  namespace reactor
  {
    namespace network
    {
      /*-------------.
      | Construction |
      `-------------*/
      FingerprintedSocket::FingerprintedSocket(
        SSLEndPoint const& endpoint,
        std::vector<unsigned char>  fingerprint,
        DurationOpt timeout):
          SSLSocket(endpoint, timeout),
          _fingerprint(std::move(fingerprint))
      {
        this->_check_certificate();
      }

      FingerprintedSocket::FingerprintedSocket(
        const std::string& hostname,
        const std::string& port,
        std::vector<unsigned char> const& fingerprint,
        DurationOpt timeout):
          FingerprintedSocket(resolve_tcp(hostname, port), fingerprint, timeout)
      {}


      FingerprintedSocket::~FingerprintedSocket()
      = default;

      /*------------------.
      | Certificate Check |
      `------------------*/
      void
      FingerprintedSocket::_check_certificate()
      {
        ELLE_DEBUG("certificate verification");
        SSL *ssl = this->_socket->native_handle();
        X509 *cert = SSL_get_peer_certificate(ssl);

        ELLE_DEBUG("ssl obj is %s", ssl);
        ELLE_DEBUG("cert is %s", cert);

        // Get fingerprint of peer certificate in sha1.
        unsigned int peer_fingerprint_size;
        unsigned char peer_fingerprint[EVP_MAX_MD_SIZE];
        X509_digest(cert, EVP_sha1(), peer_fingerprint, &peer_fingerprint_size);
        X509_free(cert);

        // Comparison of both fingerprints.
        if (peer_fingerprint_size != this->_fingerprint.size())
        {
          throw SSLCertificateError("peer fingerprint has incorrect length");
        }

        // Compare sha1s
        if (memcmp(peer_fingerprint,
                   this->_fingerprint.data(),
                   this->_fingerprint.size()) == 0)
        {
          ELLE_TRACE("certificate verified");
        }
        else
        {
          throw SSLCertificateError("peer fingerprint does not match local");
        }
      }
    }
  }
}
