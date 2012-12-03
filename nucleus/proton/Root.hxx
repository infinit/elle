#ifndef NUCLEUS_PROTON_ROOT_HXX
# define NUCLEUS_PROTON_ROOT_HXX

/*-------------.
| Serializable |
`-------------*/

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Root,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._block;
  archive & value._height;
  archive & value._capacity;
}

#endif
