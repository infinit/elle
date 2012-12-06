#include <nucleus/proton/Nature.hh>
#include <nucleus/Exception.hh>

#include <iostream>

namespace nucleus
{
  namespace proton
  {
    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Nature const nature)
    {
      switch (nature)
        {
        case Nature::data_seam:
          {
            stream << "data(seam)";
            break;
          }
        case Nature::data_quill:
          {
            stream << "data(quill)";
            break;
          }
        case Nature::data_value:
          {
            stream << "data(value)";
            break;
          }
        case Nature::catalog_seam:
          {
            stream << "catalog(seam)";
            break;
          }
        case Nature::catalog_quill:
          {
            stream << "catalog(quill))";
            break;
          }
        case Nature::catalog_value:
          {
            stream << "catalog(value)";
            break;
          }
        case Nature::reference_seam:
          {
            stream << "reference(seam)";
            break;
          }
        case Nature::reference_quill:
          {
            stream << "reference(quill)";
            break;
          }
        case Nature::reference_value:
          {
            stream << "reference(value)";
            break;
          }
        default:
          {
            throw Exception("unknown nature: '%s'", static_cast<int>(nature));
          }
        }

      return (stream);
    }
  }
}
