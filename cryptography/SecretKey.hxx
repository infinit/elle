#ifndef  ELLE_CRYPTOGRAPHY_SECRETKEY_HXX
# define ELLE_CRYPTOGRAPHY_SECRETKEY_HXX

# include <elle/standalone/Report.hh>

# include <elle/utility/Buffer.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
  namespace cryptography
  {

    template<typename T> Status
      SecretKey::Encrypt(T const& in, Cipher& out) const
      {
        static_assert(
            !std::is_same<T, elle::utility::Buffer>::value,
            "Explicitly cast to WeakBuffer needed"
        );

        elle::utility::Buffer buf;
        try
          {
            buf.Writer() << in;
          }
        catch (std::exception const& err)
          {
            escape("Cannot save object: %s", err.what());
          }

        return this->Encrypt(
            elle::utility::WeakBuffer(buf),
            out
        );
      }

    template<typename T> Status
      SecretKey::Decrypt(Cipher const& in, T& out) const
      {
        elle::utility::Buffer buffer;

        if (this->Decrypt(in, buffer) == elle::Status::Error)
          escape("Cannot decrypt cipher");

        try
          {
            buffer.Reader() >> out;
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

# include <elle/idiom/Close.hh>
#  include <cassert>
# include <elle/idiom/Open.hh>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::SecretKey,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.region;
}

#endif
