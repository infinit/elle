#ifndef SATELLITES_DIARY_MEMOIRSSERIALIZER_HXX
# define SATELLITES_DIARY_MEMOIRSSERIALIZER_HXX


# include "Memoirs.hh"

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE_SERIALIZER(satellite::Memoirs,
                                 archive,
                                 memoirs,
                                 version)
{
  (void) version;
  throw "not implemented";
}


#endif
