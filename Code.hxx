#ifndef ELLE_CRYPTOGRAPHY_CODE_HXX
# define ELLE_CRYPTOGRAPHY_CODE_HXX

# include <cassert>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(elle::cryptography::Code,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.region;
  // std::cerr << "CODE SERIALIZATION: " << value.region.size << std::endl;
}

#endif
