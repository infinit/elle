#include <reactor/network/fingerprinted-socket.hh>
#include <reactor/network/exception.hh>

#include <openssl/ssl.h>
#include <openssl/x509.h>
#include <openssl/evp.h>

ELLE_LOG_COMPONENT("reactor.network.FingerPrintedSocket");

namespace reactor
{
  namespace network
  {
    /*-------------.
    | Construction |
    `-------------*/
    FingerprintedSocket::FingerprintedSocket(
      reactor::Scheduler& sched,
      SSLEndPoint const& endpoint,
      SSLCertificate const& certificate,
      std::vector<unsigned char> const& fingerprint,
      Handshake_type type,
      DurationOpt timeout):
        SSLSocket(sched, endpoint, certificate, type, timeout),
        _fingerprint(fingerprint)
    {
      this->_check_certificate();
    }

    FingerprintedSocket::FingerprintedSocket(
      SSLEndPoint const& endpoint,
      SSLCertificate const& certificate,
      std::vector<unsigned char> const& fingerprint,
      DurationOpt timeout,
      Handshake_type type):
        FingerprintedSocket(*reactor::Scheduler::scheduler(),
                            endpoint,
                            certificate,
                            fingerprint,
                            type,
                            timeout)
    {}

    FingerprintedSocket::FingerprintedSocket(
      const std::string& hostname,
      const std::string& port,
      SSLCertificate const& certificate,
      std::vector<unsigned char> const& fingerprint,
      Handshake_type type,
      DurationOpt timeout):
        SSLSocket(hostname, port, certificate, type, timeout),
        _fingerprint(fingerprint)
    {
      this->_check_certificate();
    }


    FingerprintedSocket::~FingerprintedSocket()
    {}

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
        ELLE_LOG("certificate verified");
      }
      else
      {
        throw SSLCertificateError("peer fingerprint does not match local");
      }
    }
  }
}