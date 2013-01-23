#ifndef NUCLEUS_PROTON_MODE_HH
# define NUCLEUS_PROTON_MODE_HH

# include <ostream>

namespace nucleus
{
  namespace proton
  {
    /*-------------.
    | Enumerations |
    `-------------*/

    /// Define the mode of the contents i.e the state of its embedded node
    /// being either encrypted or decrypted.
    enum class Mode
      {
        encrypted,
        decrypted
      };

    /*----------.
    | Operators |
    `----------*/

    std::ostream&
    operator <<(std::ostream& stream,
                Mode const mode);
  }
}

#endif
