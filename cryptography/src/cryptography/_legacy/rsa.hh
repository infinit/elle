#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)
# ifndef INFINIT_CRYPTOGRAPHY_LEGACY_RSA_HH
#  define INFINIT_CRYPTOGRAPHY_LEGACY_RSA_HH

/*------.
| Class |
`------*/

namespace infinit
{
  namespace cryptography
  {
    namespace legacy
    {
      namespace rsa
      {
        class Dummy:
          public elle::serialize::SerializableMixin<Dummy>,
          public elle::serialize::DynamicFormat<Dummy>
        {};
      }
    }
  }
}

/*----------.
| Serialize |
`----------*/

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::legacy::rsa::Dummy,
                      archive,
                      value,
                      format)
{}

# endif

#else
# warning "LEGACY: this file should not have been included"
#endif
