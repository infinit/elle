#ifndef INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX
# define INFINIT_CRYPTOGRAPHY_SECRETKEY_HXX

# include <elle/Buffer.hh>

# include <elle/idiom/Open.hh>

namespace infinit
{
  namespace cryptography
  {

    template<typename T> elle::Status
      SecretKey::Encrypt(T const& in, Cipher& out) const
      {
        static_assert(
            !std::is_same<T, elle::Buffer>::value,
            "Explicitly cast to WeakBuffer needed"
        );

        elle::Buffer buf;
        try
          {
            buf.writer() << in;
          }
        catch (std::exception const& err)
          {
            escape("Cannot save object: %s", err.what());
          }

        return this->Encrypt(
            elle::WeakBuffer(buf),
            out
        );
      }

    template<typename T> elle::Status
      SecretKey::Decrypt(Cipher const& in, T& out) const
      {
        elle::Buffer buffer;

        if (this->Decrypt(in, buffer) == elle::Status::Error)
          escape("Cannot decrypt cipher");

        try
          {
            buffer.reader() >> out;
          }
        catch (std::exception const& err)
          {
            escape("Cannot decode object: %s", err.what());
          }

        return elle::Status::Ok;
      }
  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::SecretKey,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.region;
}

#endif
