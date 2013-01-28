#include <elle/assert.hh>

#include <cryptography/SecretKey.hh>
#include <cryptography/KeyPair.hh>
#include <cryptography/Cipher.hh>
// XXX[temporary: for cryptography]
using namespace infinit;

#include <iostream>

struct A
{
public:
  A(): // XXX[to remove: instead use a load constructor]
    _d(0.0),
    _f(0.0)
  {
  }

  A(double const d,
    std::string const& s,
    float const f):
    _d(d),
    _s(s),
    _f(f)
  {
  }

  A(A const&) = default;

  bool operator == (A const& other) const
  {
    return (this->_d == other._d &&
            this->_s == other._s &&
            this->_f == other._f);
  }

  ELLE_SERIALIZE_FRIEND_FOR(A);

private:
  double _d;
  std::string _s;
  float _f;
};

ELLE_SERIALIZE_SIMPLE(A, archive, value, version)
{
  assert(version == 0);

  archive & value._d;
  archive & value._s;
  archive & value._f;
}


struct Virtual
  : public elle::serialize::Serializable<>
{
  std::string base;
};

ELLE_SERIALIZE_SIMPLE(Virtual, ar, value, version)
{
  assert(version == 0);

  ar & value.base;
}

struct Implem
  : public Virtual
  , public elle::serialize::SerializableMixin<Implem>
{
  std::string impl;
};

ELLE_SERIALIZE_SIMPLE(Implem, ar, value, version)
{
  assert(version == 0);

  ar & base_class<Virtual>(value);
  ar & value.impl;
}

int main()
{
  std::string const secret_string =
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
    "Nobody should read this, it's only here for testing purposes."
  ;

  cryptography::SecretKey secret_key{cryptography::SecretKey::generate(256)};

  cryptography::Cipher cipher{secret_key.encrypt(secret_string)};

  {
    std::string res = secret_key.decrypt<std::string>(cipher);

    ELLE_ASSERT(res == secret_string);
  }

  {
    A a{42.0, "hey ho", 12.2f};

    cryptography::Cipher cipher{secret_key.encrypt(a)};

    A res{secret_key.decrypt<A>(cipher)};

    ELLE_ASSERT(res == a);
  }

  {
    Implem impl;
    impl.base = "paf";
    impl.impl = "pif";
    Virtual& virt = impl;

    cryptography::Cipher c1{secret_key.encrypt(impl)};
    cryptography::Cipher c2{secret_key.encrypt(impl)};
    cryptography::Cipher c3{secret_key.encrypt(virt)};
    cryptography::Cipher c4{secret_key.encrypt(virt)};

    Implem res1{secret_key.decrypt<Implem>(c1)};
    Implem res2{secret_key.decrypt<Implem>(c2)};
    Implem res3{secret_key.decrypt<Implem>(c3)};
    Implem res4{secret_key.decrypt<Implem>(c4)};

    Virtual& res4_ref = res4;

    res4_ref = secret_key.decrypt<Implem>(c4);

    ELLE_ASSERT(res1.base == "paf" && res1.impl == "pif");
    ELLE_ASSERT(res2.base == "paf" && res2.impl == "pif");
    ELLE_ASSERT(res3.base == "paf" && res3.impl == "pif");
    ELLE_ASSERT(res4.base == "paf" && res4.impl == "pif");
  }

  {
    cryptography::SecretKey key{elle::String("")};
    cryptography::KeyPair kp =
      cryptography::KeyPair::generate(cryptography::Cryptosystem::rsa, 1024);
    cryptography::Cipher* cipher =
      new cryptography::Cipher{key.encrypt(kp.k())};
    cryptography::PrivateKey* k =
      new cryptography::PrivateKey{
        key.decrypt<cryptography::PrivateKey>(*cipher)};

    ELLE_ASSERT(*k == kp.k());
  }

  std::cout << "tests done.\n";

  return (0);
}
