#ifndef  LUNE_PHRASESERIALIZER_HXX
# define LUNE_PHRASESERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <lune/Phrase.hh>

ELLE_SERIALIZE_SIMPLE(lune::Phrase,
                      archive,
                      value,
                      version)
{
  assert(version == 0);

  archive & value.port;
  archive & value.pass;
}

#endif
