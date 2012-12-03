#ifndef NUCLEUS_PROTON_RADIX_HXX
# define NUCLEUS_PROTON_RADIX_HXX

//
// ---------- serialize -------------------------------------------------------
//

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Radix,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._mode;

  switch (value._mode)
    {
    case nucleus::proton::Mode::empty:
      {
        break;
      }
    case nucleus::proton::Mode::value:
      {
        archive & elle::serialize::alive_pointer(value._cipher);

        break;
      }
    case nucleus::proton::Mode::block:
      {
        archive & elle::serialize::alive_pointer(value._address);

        break;
      }
    case nucleus::proton::Mode::tree:
      {
        archive & elle::serialize::alive_pointer(value._root);

        break;
      }
    default:
      throw Exception("unknown radix mode '%s'", value._mode);
    }
}

#endif
