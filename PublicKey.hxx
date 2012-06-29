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

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/cryptography/LargeSerializer.hxx>

# include <elle/cryptography/PublicKey.hh>

ELLE_SERIALIZE_SPLIT(elle::cryptography::PublicKey)

// XXX ya moyen de faire bcp plus simple, cf BN_new
ELLE_SERIALIZE_SPLIT_LOAD(elle::cryptography::PublicKey,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

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

  assert(value.key() == nullptr);

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
}

ELLE_SERIALIZE_SPLIT_SAVE(elle::cryptography::PublicKey,
                          archive,
                          value,
                          version)
{
  assert(version == 0);
  assert(value.key() != nullptr);

  archive << *(value.key()->pkey.rsa->n)
          << *(value.key()->pkey.rsa->e);
}

#endif


