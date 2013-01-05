#ifndef NUCLEUS_PROTON_CLEF_HXX
# define NUCLEUS_PROTON_CLEF_HXX

/*-------------.
| Serializable |
`-------------*/

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Clef,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._address;
  archive & value._secret;
}

#endif
