//
// ---------- Class -----------------------------------------------------------
//

#include <elle/serialization/binary.hh>
#include <elle/utility/Move.hh>

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      template <typename T>
      elle::Buffer
      PrivateKey::sign(T const& o) const
      {
        return this->sign(
          o,
          elle::serialization::_details::serialization_tag<T>::type::version);
      }

      template <>
      inline
      elle::Buffer
      PrivateKey::sign<elle::Buffer>(elle::Buffer const& b) const
      {
        return this->sign(elle::ConstWeakBuffer(b));
      }

      template <typename T>
      elle::Buffer
      PrivateKey::sign(T const& o, elle::Version const& version) const
      {
        return this->_sign_async(o, version)(this);
      }

      template <typename T>
      std::function<elle::Buffer ()>
      PrivateKey::sign_async(T const& o, elle::Version const& version) const
      {
        return [self = this->shared_from_this(),
                sign = this->_sign_async(o, version)]
        {
          return sign(self.get());
        };
      }

      template <typename T>
      std::function<elle::Buffer (PrivateKey const* self)>
      PrivateKey::_sign_async(T const& o, elle::Version const& version) const
      {
        ELLE_LOG_COMPONENT("elle.cryptography.rsa.PrivateKey");
        ELLE_TRACE_SCOPE("%s: sign %s", *this, o);
        return [serialized =
                  elle::serialization::binary::serialize(o, version, false),
                version] (PrivateKey const* self)
        {
          elle::Buffer res;
          {
            elle::IOStream output(res.ostreambuf());
            elle::serialization::binary::serialize(version, output, false);
            ELLE_DUMP("serialization: %s", serialized);
            auto signature = self->sign(serialized);
            ELLE_DUMP("signature: %s", signature);
            ELLE_DUMP("version: %s", version);
            output.write(reinterpret_cast<char const*>(signature.contents()),
                         signature.size());
          }
          return res;
        };
      }
    }
  }
}
