#ifndef ELLE_VERSION_HXX
# define ELLE_VERSION_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(elle::Version,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._major;
  archive & value._minor;
}

#endif
