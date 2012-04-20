#ifndef  ELLE_UTILITY_DURATIONSERIALIZER_HXX
# define ELLE_UTILITY_DURATIONSERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/utility/Duration.hh>

ELLE_SERIALIZE_SIMPLE(elle::utility::Duration,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.unit;
  archive & value.value;
}

#endif
