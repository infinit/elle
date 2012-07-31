
#include <iostream>

#include <elle/Elle.hh>
#include <elle/cryptography/SecretKey.hh>
#include <elle/cryptography/Cipher.hh>

struct A
{
  double d;
  std::string s;
  float f;
  bool operator == (A const& other) const
  {
    return (
        this->d == other.d &&
        this->s == other.s &&
        this->f == other.f
    );
  }
};

ELLE_SERIALIZE_SIMPLE(A, archive, value, version)
{
  archive & value.d;
  archive & value.s;
  archive & value.f;
}


struct Virtual
  : public elle::concept::contract::Serializable<elle::serialize::BufferArchive>
{
  std::string base;
};

ELLE_SERIALIZE_SIMPLE(Virtual, ar, value, version)
{
  ar & value.base;
}

struct Implem
  : public Virtual
  , public elle::concept::Serializable<Implem, elle::serialize::BufferArchive>
{
  std::string impl;
};

ELLE_SERIALIZE_SIMPLE(Implem, ar, value, version)
{
  ar & static_cast<Virtual&>(value);
  ar & value.impl;
}

int main()
{
  assert(elle::Elle::Initialize() == elle::Status::Ok);

  elle::cryptography::SecretKey secret_key;

  assert(secret_key.Generate() == elle::Status::Ok);

  elle::cryptography::Cipher cipher;

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

  assert(secret_key.Encrypt(secret_string, cipher) == elle::Status::Ok);

    {
      std::string res;
      assert(secret_key.Decrypt(cipher, res) == elle::Status::Ok);

      assert(res == secret_string);
    }

    {
      A a{42.0, "hey ho", 12.2f};
      elle::cryptography::Cipher cipher;
      assert(secret_key.Encrypt(a, cipher) == elle::Status::Ok);

      A res;
      assert(secret_key.Decrypt(cipher, res) == elle::Status::Ok);

      assert(res == a);
    }

    {
      Implem impl;
      impl.base = "paf";
      impl.impl = "pif";
      Virtual& virt = impl;

      elle::cryptography::Cipher c1, c2, c3, c4;
      assert(secret_key.Encrypt(impl, c1) == elle::Status::Ok);
      assert(secret_key.Encrypt(elle::serialize::serializable(impl), c2) == elle::Status::Ok);
      assert(secret_key.Encrypt(elle::serialize::serializable(virt), c3) == elle::Status::Ok);
      assert(secret_key.Encrypt(elle::serialize::serializable(virt), c4) == elle::Status::Ok);

      Implem res1, res2, res3, res4;
      Virtual& res4_ref = res4;
      assert(secret_key.Decrypt(c1, res1) == elle::Status::Ok);
      assert(secret_key.Decrypt(c2, res2) == elle::Status::Ok);
      assert(secret_key.Decrypt(c3, res3) == elle::Status::Ok);

      elle::serialize::Serializable<Virtual> s(res4_ref);
      assert(secret_key.Decrypt(c4, s) == elle::Status::Ok);

      assert(res1.base == "paf" && res1.impl == "pif");
      assert(res2.base == "paf" && res2.impl == "pif");
      assert(res3.base == "paf" && res3.impl == "pif");
      assert(res4.base == "paf" && res4.impl == "pif");
    }

  std::cout << "tests done.\n";

  return 0;

}
