#ifndef  ELLE_CRYPTOGRAPHY_SECRETKEY_HXX
# define ELLE_CRYPTOGRAPHY_SECRETKEY_HXX

# include <elle/standalone/Maid.hh>
# include <elle/standalone/Report.hh>

# include <elle/utility/Buffer.hh>
# include <elle/serialize/BufferArchive.hh>
# include <elle/cryptography/PlainSerializer.hxx>

# include <elle/idiom/Open.hh>

namespace elle
{
  using namespace standalone;

  namespace cryptography
  {

    template<typename T> Status
      SecretKey::Encrypt(T const& in, Cipher& out) const
      {
        elle::utility::Buffer buf;
        static_assert(
            !std::is_same<T, elle::utility::Buffer>::value,
            "Explicitly cast to WeakBuffer needed"
        );

        try
          {
            buf.Writer() << in;
            std::cout << "to encrypt size: " << buf.Size() << std::endl;
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

# include <elle/idiom/Close.hh>

#endif
