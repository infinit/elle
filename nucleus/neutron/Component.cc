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
        case ComponentContents:
          {
            stream << "contents";
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
        case ComponentSeamCatalog:
          {
            stream << "seam(catalog)";
            break;
          }
        case ComponentQuillCatalog:
          {
            stream << "quill(catalog)";
            break;
          }
          // XXX
        default:
          {
            throw Exception("unknown component: '%s'", component);
          }
        }

      return (stream);
    }

  }
}
