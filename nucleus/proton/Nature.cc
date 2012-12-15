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
            stream << "seam<data>";
            break;
          }
        case Nature::data_quill:
          {
            stream << "quill<data>";
            break;
          }
        case Nature::data_value:
          {
            stream << "value<data>";
            break;
          }
        case Nature::catalog_seam:
          {
            stream << "seam<catalog>";
            break;
          }
        case Nature::catalog_quill:
          {
            stream << "quill<catalog>";
            break;
          }
        case Nature::catalog_value:
          {
            stream << "value<catalog>";
            break;
          }
        case Nature::reference_seam:
          {
            stream << "seam<reference>";
            break;
          }
        case Nature::reference_quill:
          {
            stream << "quill<reference>";
            break;
          }
        case Nature::reference_value:
          {
            stream << "value<reference>";
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
