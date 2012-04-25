#ifndef ELLE_CRYPTOGRAPHY_PRIVATEKEY_HXX
# define ELLE_CRYPTOGRAPHY_PRIVATEKEY_HXX

# include <elle/cryptography/Digest.hh>

# include <elle/standalone/Maid.hh>
# include <elle/standalone/Report.hh>

# include <elle/utility/Buffer.hh>
# include <elle/serialize/BufferArchive.hh>

//# include <elle/cryptography/CodeSerializer.hxx>
# include <elle/standalone/RegionSerializer.hxx>

# include <elle/idiom/Open.hh>

namespace elle
{
  using namespace standalone;

  namespace cryptography
  {

    template<typename T> Status
      PrivateKey::Decrypt(Code const& in, T& out) const
      {
        elle::utility::Buffer out_buffer;

        if (this->Decrypt(in, out_buffer) == elle::Status::Error)
          escape("Cannot decrypt code");

        try
          {
            out_buffer.Reader() >> out;
          }
        catch (std::exception const& err)
          {
            escape("Cannot decrypt the object: %s", err.what());
          }

        return elle::Status::Ok;
      }

    template<typename T>  Status
      PrivateKey::Encrypt(T const& in, Code& out) const
      {
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

    template<typename T>  Status
      PrivateKey::Sign(T const& in, Signature& out) const
      {
        elle::utility::Buffer buf;

        try
          {
            buf.Writer() << in;
          }
        catch (std::exception const& err)
          {
            escape("Cannot save object: %s", err.what());
          }

        return this->Sign(elle::utility::WeakBuffer(buf), out);
      }

  }
}

#endif
