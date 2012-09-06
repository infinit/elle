#ifndef  ELLE_UTILITY_TIME_HXX
# define ELLE_UTILITY_TIME_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(elle::utility::Time,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.nanoseconds;
}

#endif
