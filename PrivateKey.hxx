#ifndef ELLE_CRYPTOGRAPHY_PRIVATEKEY_HXX
# define ELLE_CRYPTOGRAPHY_PRIVATEKEY_HXX

# include <elle/standalone/Report.hh>

# include <elle/utility/Buffer.hh>
# include <elle/serialize/BufferArchive.hh>

# include <elle/idiom/Open.hh>

namespace elle
{
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

    template<typename T>  Status
      PrivateKey::Sign(T const& in, Signature& out) const
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

        return this->Sign(elle::utility::WeakBuffer(buf), out);
      }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <cassert>
# include <stdexcept>

# include <elle/serialize/ArchiveSerializer.hxx>

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
