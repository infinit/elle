#include <nucleus/proton/Breed.hh>

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
                Breed const breed)
    {
      switch (breed)
        {
        case Breed::data_seam:
          {
            stream << "data(seam)";
            break;
          }
        case Breed::data_quill:
          {
            stream << "data(quill)";
            break;
          }
        case Breed::data_value:
          {
            stream << "data(value)";
            break;
          }
        case Breed::catalog_seam:
          {
            stream << "catalog(seam)";
            break;
          }
        case Breed::catalog_quill:
          {
            stream << "catalog(quill))";
            break;
          }
        case Breed::catalog_value:
          {
            stream << "catalog(value)";
            break;
          }
        case Breed::reference_seam:
          {
            stream << "reference(seam)";
            break;
          }
        case Breed::reference_quill:
          {
            stream << "reference(quill)";
            break;
          }
        case Breed::reference_value:
          {
            stream << "reference(value)";
            break;
          }
        default:
          {
            throw Exception("unknown breed: '%s'", static_cast<int>(breed));
          }
        }

      return (stream);
    }
  }
}
