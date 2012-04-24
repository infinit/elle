#ifndef  ELLE_CRYPTOGRAPHY_SECRETKEY_HXX
# define ELLE_CRYPTOGRAPHY_SECRETKEY_HXX

# include <elle/standalone/Maid.hh>
# include <elle/standalone/Report.hh>

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
        Clear clear;
        if (this->Decrypt(in, clear) == elle::Status::Error)
          escape("Cannot decrypt cipher");
        try
          {
            elle::utility::WeakBuffer buf(clear.contents, clear.size);
            buf.Reader() >> out;
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
