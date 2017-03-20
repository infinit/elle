//
// ---------- class -----------------------------------------------------------
//

#include <elle/Lazy.hh>
#include <elle/serialization/binary.hh>
#include <elle/serialization/json.hh>
#include <elle/utility/Move.hh>

namespace elle
{
  namespace cryptography
  {
    namespace rsa
    {
      template <>
      inline
      bool
      PublicKey::verify<elle::Buffer>(elle::ConstWeakBuffer const& signature,
                                      elle::Buffer const& plain) const
      {
        return this->verify(signature, elle::ConstWeakBuffer(plain));
      }

      template <typename T>
      bool
      PublicKey::verify(elle::ConstWeakBuffer const& signature,
                        T const& o) const
      {
        auto data = this->_verify_data(signature, o);
        return this->_verify(data.first, data.second);
      }

      template <typename T>
      std::function <bool ()>
      PublicKey::verify_async(elle::ConstWeakBuffer const& signature,
                              T const& o) const
      {
        return [self = this->shared_from_this(),
                data = this->_verify_data(signature, o)]
        {
          return self->_verify(data.first, data.second);
        };
      }

      template <typename T>
      std::pair<elle::Buffer, elle::Buffer>
      PublicKey::_verify_data(elle::ConstWeakBuffer const& signature,
                              T const& o) const
      {
        ELLE_LOG_COMPONENT("elle.cryptography.rsa.PublicKey");
        ELLE_TRACE_SCOPE("%s: verify %s", this, o);
        elle::IOStream input(signature.istreambuf());
        auto version =
          elle::serialization::binary::deserialize<elle::Version>(input, false);
        auto s = elle::Buffer(std::string{std::istreambuf_iterator<char>(input),
                                          std::istreambuf_iterator<char>()});
        auto serialized =
          elle::serialization::binary::serialize(o, version, false);
        ELLE_DUMP(
          "data: %s",
          elle::lazy([&] {
              std::stringstream out;
              elle::serialization::json::serialize(o, out, version,
                                                   false, true);
              return out.str();
            }));
        ELLE_DUMP("serialization version: %s", version);
        ELLE_DUMP("serialized data: %s", serialized);
        ELLE_DUMP("signature: %s", s);
        return std::make_pair(std::move(s), std::move(serialized));
      }
    }
  }
}

//
// ---------- Hash ------------------------------------------------------------
//

#include <elle/serialization/binary.hh>

namespace std
{
  template <>
  struct hash<elle::cryptography::rsa::PublicKey>
  {
    size_t
    operator ()(elle::cryptography::rsa::PublicKey const& value) const
    {
      std::stringstream stream;
      {
        elle::serialization::binary::SerializerOut output(stream);
        output.serialize("value", value);
      }

      size_t result = std::hash<std::string>()(stream.str());

      return (result);
    }
  };
}
