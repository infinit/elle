#ifndef  LUNE_SETSERIALIZER_HXX
# define LUNE_SETSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/VectorSerializer.hxx>
# include <elle/network/LocusSerializer.hh>

# include <lune/Set.hh>

ELLE_SERIALIZE_SIMPLE(lune::Set,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.loci;
}

#endif
