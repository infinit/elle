#ifndef ELLE_VERSION_HXX
# define ELLE_VERSION_HXX

# include <elle/serialize/Serializer.hh>

namespace elle
{
  constexpr
  Version::Version(elle::Natural8 major,
                   elle::Natural8 minor,
                   elle::Natural8 subminor):
    _major(major),
    _minor(minor),
    _subminor(subminor)
  {}
}

ELLE_SERIALIZE_STATIC_FORMAT(elle::Version, 1);

ELLE_SERIALIZE_SPLIT(elle::Version);

ELLE_SERIALIZE_SPLIT_SAVE(elle::Version,
                          archive,
                          value,
                          version)
{
  archive & value._major;
  archive & value._minor;
  if (version >= 1)
    archive & value._subminor;
}

ELLE_SERIALIZE_SPLIT_LOAD(elle::Version,
                          archive,
                          value,
                          version)
{
  archive & value._major;
  archive & value._minor;
  if (version >= 1)
    archive & value._subminor;
  else
    value._subminor = 0;
}

#endif
