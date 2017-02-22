#ifndef ELLE_VERSION_HXX
# define ELLE_VERSION_HXX

namespace elle
{
  constexpr
  Version::Version(elle::Natural8 major,
                   elle::Natural8 minor,
                   elle::Natural8 subminor)
    : _major(major)
    , _minor(minor)
    , _subminor(subminor)
  {}
}

#endif
