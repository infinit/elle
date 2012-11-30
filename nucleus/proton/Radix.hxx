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
  archive & value._capacity;

  switch (value._mode)
    {
    case nucleus::proton::Radix::Mode::empty:
      {
        break;
      }
    case nucleus::proton::Radix::Mode::value:
      {
        archive & elle::serialize::alive_pointer(value._value);

        break;
      }
    case nucleus::proton::Radix::Mode::block:
      {
        archive & elle::serialize::alive_pointer(value._block);

        break;
      }
    case nucleus::proton::Radix::Mode::tree:
      {
        archive & elle::serialize::alive_pointer(value._tree);

        break;
      }
    default:
      throw Exception("unknown radix mode '%s'", value._mode);
    }
}

#endif
