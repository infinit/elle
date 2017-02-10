#include <cryptography/hotp.hh>
#include <cryptography/hash.hh>
#include <cryptography/random.hh>

namespace elle
{
  namespace cryptography
  {
    static const int sha1_length = 20;

    HOTPClient::HOTPClient(elle::ConstWeakBuffer const& key, int c)
    : _c(c)
    , _key(key.contents(), key.size())
    {
      ELLE_ASSERT_EQ(signed(_key.size()), sha1_length);
    }

    int32_t hotp(elle::Buffer const& k, int64_t c)
    {
      // hotp: https://tools.ietf.org/html/rfc4226
      // hmac: https://tools.ietf.org/html/rfc2104
      elle::Buffer k2 = k;
      for (unsigned i=0; i<k2.size(); ++i)
        (unsigned char&)(k2[i]) ^= 0x36;
      k2.append(&c, 8);
      elle::Buffer h1 = hash(k2, Oneway::sha1);
      elle::Buffer k1 = k;
      for (unsigned i=0; i<k1.size(); ++i)
        (unsigned char&)(k1[i]) ^= 0x5c;
      k1.append(h1.contents(), h1.size());
      elle::Buffer h = hash(k1, Oneway::sha1);
      ELLE_ASSERT_EQ(signed(h.size()), sha1_length);
      int offset = h[19] & 0x0F;
      return (h[offset]
        + (h[offset+1] << 8)
        + (h[offset+2] << 16)
        + (h[offset+3] << 24)) & 0x7FFFFFFF;
    }

    int32_t HOTPClient::next()
    {
      return hotp(_key, ++_c);
    }

    HOTPServer::HOTPServer(elle::ConstWeakBuffer key, int c, int seek)
    : _c(c)
    , _seek(seek)
    , _key(key.contents(), key.size())
    , _seen_pos(0)
    {
      if (key.size() == 0)
      {
        elle::Buffer password(random::generate<elle::Buffer>(20));
        _key = hash(password,
                    Oneway::sha1);
      }
      _seen.resize(_seek * 2, -1); // -1 is not a possible hotp value
    }

    HOTPServer::ValidationResult HOTPServer::validate(int32_t v)
    {
      if (std::find(_seen.begin(), _seen.end(), v) != _seen.end())
        return ValidationResult::replay;
      _seen[_seen_pos] = v;
      _seen_pos = (_seen_pos + 1) % _seen.size();
      int m = hotp(_key, ++_c);
      if (m == v)
        return ValidationResult::ok;
      // check values around
      for (int i = _c - _seek; i<= _c + _seek; ++i)
      {
        if (hotp(_key, i) == v)
        {
          // assuming more drops than reordering, it computationaly makes sense
          // to reset _c to where it matched.
          _c = i;
          return ValidationResult::ok;
        }
      }
      --_c;
      return ValidationResult::not_found;
    }

    std::ostream& operator << (std::ostream& o, HOTPServer::ValidationResult const& vr)
    {
      switch(vr)
      {
      case HOTPServer::ValidationResult::ok:
        return o << "ok"; break;
      case HOTPServer::ValidationResult::replay:
        return o << "replay"; break;
      case HOTPServer::ValidationResult::not_found:
        return o << "not_found"; break;
      }
      return o;
    }
  }
}
