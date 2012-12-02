#ifndef NUCLEUS_PROTON_ROOT_HXX
# define NUCLEUS_PROTON_ROOT_HXX

/*-------------.
| Serializable |
`-------------*/

ELLE_SERIALIZE_SPLIT(nucleus::proton::Root);

ELLE_SERIALIZE_SPLIT_LOAD(nucleus::proton::Root,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  archive >> value._height;
  archive >> value._capacity;
  archive >> value._block;
}

ELLE_SERIALIZE_SPLIT_SAVE(nucleus::proton::Root,
                          archive,
                          value,
                          version)
{
  enforce(version == 0);

  // XXX[expliquer le fait qu'il ne faut pas avoir de key dans ce handle]
  enforce(value._block.secret() == cryptography::SecretKey::Null);

  archive << value._height;
  archive << value._capacity;
  archive << value._block;
}

#endif
