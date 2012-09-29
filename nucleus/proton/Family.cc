#include <nucleus/proton/Family.hh>

#include <elle/printf.hh>

#include <iostream>
#include <stdexcept>

namespace nucleus
{
  namespace proton
  {

    std::ostream&
    operator <<(std::ostream& stream,
                Family const family)
    {
      switch (family)
        {
        case FamilyUnknown:
          {
            stream << "unknown";
            break;
          }
        case FamilyContentHashBlock:
          {
            stream << "content hash block";
            break;
          }
        case FamilyPublicKeyBlock:
          {
            stream << "public key block";
            break;
          }
        case FamilyOwnerKeyBlock:
          {
            stream << "owner key block";
            break;
          }
        case FamilyImprintBlock:
          {
            stream << "imprint block";
            break;
          }
        default:
          throw std::runtime_error(
            elle::sprintf("invalid family: '%s'", family));
        }

      return (stream);
    }

  }
}
