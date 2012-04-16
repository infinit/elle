#ifndef  ELLE_CRYPTOGRAPHY_PUBLIC_KEY_SERIALIZER_HXX
# define ELLE_CRYPTOGRAPHY_PUBLIC_KEY_SERIALIZER_HXX

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SPLIT(elle::cryptography::PublicKey)

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
          ::BN_clear_free(l);
        }
    };
  typedef std::unique_ptr<Large, LargeDeleter> LargePtr;

  // Temp values on the stack.
  Large n_;
  Large e_;

  archive >> n_;
  archive >> e_;

  // Dup for the RSA assignement in PublicKey::Create(Large*, Large*)
  LargePtr n(::BN_dup(&n_));
  LargePtr e(::BN_dup(&e_));

  ::BN_clear(&n_);
  ::BN_clear(&e_);

  if (!n || !e)
    throw std::bad_alloc();


  if (value->Create(::BN_dup(n.get()),
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

#endif


