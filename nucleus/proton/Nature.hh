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
        reference_value,
        attributes_seam,
        attributes_quill,
        attributes_value,
        access_seam,
        access_quill,
        access_value,
        ensemble_seam,
        ensemble_quill,
        ensemble_value
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
