#ifndef  ELLE_CRYPTOGRAPHY_PUBLICKEY_HXX
# define ELLE_CRYPTOGRAPHY_PUBLICKEY_HXX

# include <elle/standalone/Report.hh>

# include <elle/Buffer.hh>

namespace elle
{
  namespace cryptography
  {

    template<typename T>
      Status PublicKey::Encrypt(T const& in, Code& out) const
      {
        static_assert(
            !std::is_same<T, elle::Buffer>::value,
            "explicit cast to WeakBuffer needed"
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

    template <typename T>
      Status PublicKey::Decrypt(Code const& in, T& out) const
      {
        elle::Buffer buf;

        if (this->Decrypt(in, buf) == elle::Status::Error)
          escape("Cannot decrypt data");

        try
          {
            buf.reader() >> out;
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
            !std::is_same<T, elle::Buffer>::value,
            "explicit cast to WeakBuffer needed"
        );

        elle::Buffer buf;

        try
          {
            buf.writer() << any;
          }
        catch (std::exception const& err)
          {
            escape("Cannot save object: %s", err.what());
          }

        return this->Verify(
            signature,
            elle::WeakBuffer(buf)
        );
      }
  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>

# include <elle/cryptography/LargeSerializer.hxx>

ELLE_SERIALIZE_SPLIT(elle::cryptography::PublicKey)

// XXX ya moyen de faire bcp plus simple, cf BN_new
ELLE_SERIALIZE_SPLIT_LOAD(elle::cryptography::PublicKey,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  // Deleter for dupped large
  struct LargeDeleter
    {
      void operator() (Large* l)
        {
          // We didn't use BN_clear_free because public key is not a sensitive
          // data, no need to waste time in clearing it.
          ::BN_free(l);
        }
    };
  typedef std::unique_ptr<Large, LargeDeleter> LargePtr;


  LargePtr n(::BN_new());
  LargePtr e(::BN_new());

  if (!n || !e)
    throw std::bad_alloc();

  archive >> *n.get();
  archive >> *e.get();

  // XXX
  value.~PublicKey();
  new (&value) elle::cryptography::PublicKey;

  enforce(value.key() == nullptr);

  if (value.Create(n.get(),
                   e.get()) == Status::Error)
    {
      throw std::runtime_error(
          "unable to create the public key from the archive"
      );
    }
  else
    {
      n.release();
      e.release();
    }
  enforce(value.key() != nullptr);
}

ELLE_SERIALIZE_SPLIT_SAVE(elle::cryptography::PublicKey,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);
  enforce(value.key() != nullptr);

  archive << *(value.key()->pkey.rsa->n)
          << *(value.key()->pkey.rsa->e);
}

#endif
