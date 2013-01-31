#ifndef NUCLEUS_PROTON_CONTENTS_HXX
# define NUCLEUS_PROTON_CONTENTS_HXX

# include <cryptography/Code.hh>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Construction |
    `-------------*/

    template <typename T>
    Contents::Contents(Network const& network,
                       cryptography::PublicKey const& creator_K,
                       T* node):
      proton::ContentHashBlock(network, neutron::ComponentContents, creator_K),

      _nature(T::Constants::nature),
      _node(node),
      _code(nullptr)
    {
    }
  }
}

/*-------------.
| Serializable |
`-------------*/

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Contents,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::ContentHashBlock>(value);

  archive & value._nature;
  archive & elle::serialize::alive_pointer(value._code);
}

#endif
