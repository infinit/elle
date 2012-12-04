#ifndef NUCLEUS_PROTON_SHELL_HXX
# define NUCLEUS_PROTON_SHELL_HXX

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Shell::Shell(T* node):
      _breed(T::Constants::breed),
      _node(node),
      _cipher(nullptr)
    {
    }
  }
}

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/Pointer.hh>
# include <cryptography/Cipher.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Shell,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._breed;
  archive & elle::serialize::alive_pointer(value._cipher);
}

#endif
