#ifndef NUCLEUS_PROTON_CONTENTS_HXX
# define NUCLEUS_PROTON_CONTENTS_HXX

# include <cryptography/Cipher.hh>
# include <cryptography/SecretKey.hh>

namespace nucleus
{
  namespace proton
  {

//
// ---------- constructors & destructors --------------------------------------
//

    template <typename T>
    Contents::Contents(Network const& network,
                       cryptography::PublicKey const& creator_K,
                       T* node):
      proton::ContentHashBlock(network, neutron::ComponentContents, creator_K),

      _type(T::Constants::type),
      _node(node),
      _cipher(nullptr)
    {
    }

  }
}

//
// ---------- serialize -------------------------------------------------------
//

# include <elle/serialize/Serializer.hh>
# include <elle/serialize/Pointer.hh>
# include <cryptography/Cipher.hh>

ELLE_SERIALIZE_SIMPLE(nucleus::proton::Contents,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & base_class<nucleus::proton::ContentHashBlock>(value);

  archive & value._type;
  archive & elle::serialize::alive_pointer(value._cipher);
}

#endif
