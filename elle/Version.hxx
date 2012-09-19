#ifndef VERSION_HXX
# define VERSION_HXX

# include <elle/serialize/Serializer.hh>

ELLE_SERIALIZE_SIMPLE(Version,
                      archive,
                      value,
                      version)
{
  enforce(version == 0);

  archive & value._major;
  archive & value._minor;
}

#endif
