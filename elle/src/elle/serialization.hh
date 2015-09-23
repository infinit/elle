#ifndef ELLE_SERIALIZATION_TAG_HH
# define ELLE_SERIALIZATION_TAG_HH

# include <elle/Version.hh>

namespace elle
{
  struct serialization_tag
  {
    static elle::Version version;
  };
}

# include <elle/Version.hxx>

#endif
