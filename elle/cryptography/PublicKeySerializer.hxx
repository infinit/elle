#ifndef  ELLE_CRYPTOGRAPHY_PUBLIC_KEY_SERIALIZER_HXX
# define ELLE_CRYPTOGRAPHY_PUBLIC_KEY_SERIALIZER_HXX

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

  // Temp values on the stack.
  Large n_;
  Large e_;

  BN_init(&n_);
  BN_init(&e_);

  archive >> n_;
  archive >> e_;

  // Dup for the RSA assignement in PublicKey::Create(Large*, Large*)
  LargePtr n(::BN_dup(&n_));
  LargePtr e(::BN_dup(&e_));

  ::BN_free(&n_);
  ::BN_free(&e_);

  if (!n || !e)
    throw std::bad_alloc();

  // XXX
  value.~PublicKey();
  new (&value) elle::cryptography::PublicKey;

  assert(value.key() == nullptr);

  if (value.Create(::BN_dup(n.get()),
                   ::BN_dup(e.get())) == Status::Error)
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
// XXX assert(this->_key != nullptr);
// XXX[to remove]
  if (value.key() == nullptr)
    throw std::runtime_error("XXX: should not happen !");

  archive << *(value.key()->pkey.rsa->n)
          << *(value.key()->pkey.rsa->e);
}

#endif


