#ifndef ELLE_VERSION_SERIALIZATION_HXX
# define ELLE_VERSION_SERIALIZATION_HXX

# include <elle/serialize/Serializer.hh>

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
