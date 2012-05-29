#ifndef  ELLE_CRYPTOGRAPHY_PUBLICKEY_HXX
# define ELLE_CRYPTOGRAPHY_PUBLICKEY_HXX

# include <elle/standalone/Maid.hh>
# include <elle/standalone/Report.hh>

# include <elle/utility/Buffer.hh>
# include <elle/serialize/BufferArchive.hh>

namespace elle
{
  using namespace standalone;

  namespace cryptography
  {

    template<typename T>
      Status PublicKey::Encrypt(T const& in, Code& out) const
      {
        static_assert(
            !std::is_same<T, elle::utility::Buffer>::value,
            "explicit cast to WeakBuffer needed"
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

    template <typename T>
      Status PublicKey::Decrypt(Code const& in, T& out) const
      {
        elle::utility::Buffer buf;

        if (this->Decrypt(in, buf) == elle::Status::Error)
          escape("Cannot decrypt data");

        try
          {
            buf.Reader() >> out;
          }
        catch (std::exception const& err)
          {
            escape("Cannot load object: %s", err.what());
          }

        return elle::Status::Ok;
      }

    template<typename T>
      Status PublicKey::Verify(Signature const& signature, T const& any) const
      {
        static_assert(
            !std::is_same<T, elle::utility::Buffer>::value,
            "explicit cast to WeakBuffer needed"
        );

        elle::utility::Buffer buf;

        try
          {
            buf.Writer() << any;
          }
        catch (std::exception const& err)
          {
            escape("Cannot save object: %s", err.what());
          }

        return this->Verify(
            signature,
            elle::utility::WeakBuffer(buf)
        );
      }
  }
}

#endif
