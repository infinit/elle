#ifndef INFINIT_CRYPTOGRAPHY_HOTP_HH
# define INFINIT_CRYPTOGRAPHY_HOTP_HH

#include <elle/Buffer.hh>

namespace infinit
{
  namespace cryptography
  {
    int32_t
    hotp(elle::Buffer const& k, int64_t c);

    /// HMAC-OTP client
    class HOTPClient
    {
    public:
      HOTPClient(elle::ConstWeakBuffer const& key, int c = 0);
      int32_t
      next();
    private:
      ELLE_ATTRIBUTE_R(int64_t, c);
      elle::Buffer _key;
    };

    class HOTPServer
    {
    public:
      enum class ValidationResult {
        ok,
        replay,
        not_found
      };
      HOTPServer(elle::ConstWeakBuffer key = {}, int c = 0, int seek = 5);
      ValidationResult
      validate(int32_t v);
    private:
      ELLE_ATTRIBUTE_R(int64_t, c);
      int _seek;
      ELLE_ATTRIBUTE_R(elle::Buffer, key);
      std::vector<int> _seen; // sequence number seen(fixed size)
      int _seen_pos;
    };
    std::ostream& operator << (std::ostream& o, HOTPServer::ValidationResult const& vr);
  }
}

#endif
