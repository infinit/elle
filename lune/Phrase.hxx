#ifndef  LUNE_PHRASE_HXX
# define LUNE_PHRASE_HXX

# include <cassert>

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(lune::Phrase,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value.port;
  archive & value.pass;
}

#endif
