#ifndef  ELLE_CRYPTOGRAPHY_PRIVATEKEYSERIALIZER_HXX
# define ELLE_CRYPTOGRAPHY_PRIVATEKEYSERIALIZER_HXX

# include <cassert>
# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/cryptography/PrivateKey.hh>

ELLE_SERIALIZE_SPLIT(elle::cryptography::PrivateKey)

ELLE_SERIALIZE_SPLIT_SAVE(elle::cryptography::PrivateKey,
                          archive,
                          value,
                          version)
{
  assert(version == 0);

  assert(value.key() != nullptr);

  archive << *value.key()->pkey.rsa->n;
  archive << *value.key()->pkey.rsa->e;
  archive << *value.key()->pkey.rsa->d;
  archive << *value.key()->pkey.rsa->p;
  archive << *value.key()->pkey.rsa->q;
  archive << *value.key()->pkey.rsa->dmp1;
  archive << *value.key()->pkey.rsa->dmq1;
  archive << *value.key()->pkey.rsa->iqmp;
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::cryptography::PrivateKey,
                          archive,
                          value,
                          version)
{
  assert(version == 0);
  struct ScopeGuard
    {
      bool              track;
      Large*            n;
      Large*            e;
      Large*            d;
      Large*            p;
      Large*            q;
      Large*            dmp1;
      Large*            dmq1;
      Large*            iqmp;
      ScopeGuard()
        : track(true)
        , n(::BN_new())
        , e(::BN_new())
        , d(::BN_new())
        , p(::BN_new())
        , q(::BN_new())
        , dmp1(::BN_new())
        , dmq1(::BN_new())
        , iqmp(::BN_new())
      {
        if (!n || !e || !d || !p || !q || !dmp1 || !dmq1 || !iqmp)
          throw std::bad_alloc();
      }

      ~ScopeGuard()
      {
        if (!this->track)
          return;
        ::BN_clear_free(n);
        ::BN_clear_free(e);
        ::BN_clear_free(d);
        ::BN_clear_free(p);
        ::BN_clear_free(q);
        ::BN_clear_free(dmp1);
        ::BN_clear_free(dmq1);
        ::BN_clear_free(iqmp);
      }
    } guard;

  archive >> *guard.n
          >> *guard.e
          >> *guard.d
          >> *guard.p
          >> *guard.q
          >> *guard.dmp1
          >> *guard.dmq1
          >> *guard.iqmp
    ;

  // XXX because the private key is not always cleaned
  // TODO redesign cryptography classes !
  value.~PrivateKey();
  new (&value) elle::cryptography::PrivateKey();

  assert(value.key() == nullptr);

  auto res = value.Create(
      guard.n,
      guard.e,
      guard.d,
      guard.p,
      guard.q,
      guard.dmp1,
      guard.dmq1,
      guard.iqmp
  );

  if (res == elle::Status::Error)
    throw std::runtime_error("Could not create the private key !");

  guard.track = false;
}

#endif
