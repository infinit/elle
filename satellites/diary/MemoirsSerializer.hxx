#ifndef SATELLITES_DIARY_MEMOIRSSERIALIZER_HXX
# define SATELLITES_DIARY_MEMOIRSSERIALIZER_HXX

# include <elle/utility/BufferSerializer.hxx>

# include "Memoirs.hh"

ELLE_SERIALIZE_SIMPLE(satellite::Memoirs,
                      archive,
                      memoirs,
                      version)
{
  assert(version == 0);
  archive & memoirs.archive;
}

#endif
