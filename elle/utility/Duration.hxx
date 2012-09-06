#ifndef  ELLE_UTILITY_DURATION_HXX
# define ELLE_UTILITY_DURATION_HXX

# include <cassert>

# include <elle/serialize/ArchiveSerializer.hxx>

ELLE_SERIALIZE_SIMPLE(elle::utility::Duration,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);
  archive & value.unit;
  archive & value.value;
}

#endif
