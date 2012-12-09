#ifndef NUCLEUS_PROTON_RADIX_HXX
# define NUCLEUS_PROTON_RADIX_HXX

//
// ---------- serialize -------------------------------------------------------
//

# include <nucleus/proton/Address.hh>
# include <nucleus/proton/Root.hh>

# include <cryptography/Cipher.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Radix,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._strategy;

  switch (value._strategy)
    {
    case nucleus::proton::Strategy::none:
      {
        break;
      }
    case nucleus::proton::Strategy::value:
      {
        archive & elle::serialize::alive_pointer(value._cipher);

        break;
      }
    case nucleus::proton::Strategy::block:
      {
        archive & elle::serialize::alive_pointer(value._address);

        break;
      }
    case nucleus::proton::Strategy::tree:
      {
        archive & elle::serialize::alive_pointer(value._root);

        break;
      }
    default:
      throw Exception("unknown radix strategy '%s'", value._strategy);
    }
}

#endif
