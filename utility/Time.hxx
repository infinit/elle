#ifndef  ELLE_UTILITY_TIMESERIALIZER_HXX
# define ELLE_UTILITY_TIMESERIALIZER_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

# include <elle/utility/Time.hh>

ELLE_SERIALIZE_SIMPLE(elle::utility::Time,
                      archive,
                      value,
                      version)
{
  assert(version == 0);
  archive & value.nanoseconds;
}

#endif
