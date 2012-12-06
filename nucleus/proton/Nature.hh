#ifndef NUCLEUS_PROTON_NATURE_HH
# define NUCLEUS_PROTON_NATURE_HH

# include <iosfwd>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Represent the nature of the final node. This nature is used
    /// to allocate the proper node type based on such a nature.
    enum class Nature
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
                Nature const nature);
  }
}

#endif
