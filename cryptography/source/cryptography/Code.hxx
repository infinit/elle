#ifndef INFINIT_CRYPTOGRAPHY_CODE_HXX
# define INFINIT_CRYPTOGRAPHY_CODE_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(infinit::cryptography::Code,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.region;
}

#endif
