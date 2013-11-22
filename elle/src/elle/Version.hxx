#ifndef ELLE_VERSION_HXX
# define ELLE_VERSION_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_STATIC_FORMAT(elle::Version, 1);

ELLE_SERIALIZE_SIMPLE(elle::Version,
                      archive,
                      value,
                      version)
{
  archive & value._major;
  archive & value._minor;
  if (version == 1)
    archive & value._subminor;
}

#endif
