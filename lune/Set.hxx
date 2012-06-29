#ifndef  LUNE_SET_HXX
# define LUNE_SET_HXX

# include <cassert>

# include <elle/serialize/VectorSerializer.hxx>
# include <elle/network/Locus.hh>

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
