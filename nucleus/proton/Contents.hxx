#ifndef NUCLEUS_PROTON_CONTENTS_HXX
# define NUCLEUS_PROTON_CONTENTS_HXX

# include <cryptography/Cipher.hh>
# include <cryptography/SecretKey.hh>

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

      _shell(node)
    {
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Contents,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::ContentHashBlock>(value);

  archive & value._shell;
}

#endif
