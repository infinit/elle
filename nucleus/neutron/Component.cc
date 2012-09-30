#include <nucleus/neutron/Component.hh>
#include <nucleus/Exception.hh>

#include <elle/printf.hh>

#include <iostream>
#include <stdexcept>

namespace nucleus
{
  namespace neutron
  {

    std::ostream&
    operator <<(std::ostream& stream,
                Component const component)
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
          {
            throw Exception("unknown component: '%s'", component);
          }
        }

      return (stream);
    }

  }
}
