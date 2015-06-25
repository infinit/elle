#if defined(INFINIT_CRYPTOGRAPHY_LEGACY)

# ifndef INFINIT_CRYPTOGRAPHY_RSA_LEGACY_HH
#  define INFINIT_CRYPTOGRAPHY_RSA_LEGACY_HH

//
// ---------- Dummy -----------------------------------------------------------
//

/*------.
| Class |
`------*/

namespace infinit
{
  namespace cryptography
  {
    namespace legacy
    {
      class Dummy:
        public elle::serialize::SerializableMixin<Dummy>,
        public elle::serialize::DynamicFormat<Dummy>
      {};
    }
  }
}

/*--------------.
| Serialization |
`--------------*/

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::legacy::Dummy,
                      archive,
                      value,
                      format)
{}

# endif

#endif
