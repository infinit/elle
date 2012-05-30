#ifndef  XXX_INLETSERIALIZER_HXX
# define XXX_INLETSERIALIZER_HXX

# include <cassert>

# include <XXX/HandleSerializer.hxx>

# include <XXX/Inlet.hh>

ELLE_SERIALIZE_SIMPLE_T2(XXX::Inlet,
                         archive,
                         value,
                         version)
{
  assert(version == 0);

  archive & value.key;
  archive & value.value;
}

#endif
