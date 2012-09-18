#include <nucleus/neutron/Component.hh>

#include <elle/printf.hh>

#include <iostream>
#include <stdexcept>

namespace nucleus
{
  namespace neutron
  {

    std::ostream&
    operator <<(std::ostream& stream,
                Component component)
    {
      switch (component)
        {
        case ComponentUnknown:
          {
            stream << "unknown";
            break;
          }
        case ComponentObject:
          {
            stream << "object";
            break;
          }
        case ComponentData:
          {
            stream << "data";
            break;
          }
        case ComponentCatalog:
          {
            stream << "catalog";
            break;
          }
        case ComponentReference:
          {
            stream << "reference";
            break;
          }
        case ComponentAccess:
          {
            stream << "access";
            break;
          }
        case ComponentGroup:
          {
            stream << "group";
            break;
          }
        case ComponentEnsemble:
          {
            stream << "ensemble";
            break;
          }
        default:
          throw std::runtime_error(
            elle::sprintf("invalid component: '%s'", component));
        }

      return (stream);
    }

  }
}
