#ifndef  LUNE_SET_HXX
# define LUNE_SET_HXX

# include <cassert>

# include <elle/serialize/VectorSerializer.hxx>
# include <elle/network/Locus.hh>

ELLE_SERIALIZE_SIMPLE(lune::Set,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.loci;
}

#endif
