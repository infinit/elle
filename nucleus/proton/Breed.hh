#ifndef NUCLEUS_PROTON_BREED_HH
# define NUCLEUS_PROTON_BREED_HH

# include <iosfwd>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Represent all the final nodes which can be instanciated.
    /// XXX more info
    enum class Breed
      {
        data_seam,
        data_quill,
        data_value,
        catalog_seam,
        catalog_quill,
        catalog_value,
        reference_seam,
        reference_quill,
        reference_value
      };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Breed const breed);
  }
}

#endif
