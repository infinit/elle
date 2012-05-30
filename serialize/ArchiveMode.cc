
#include <ostream>

#include "ArchiveMode.hh"

inline std::ostream& operator <<(std::ostream& out, elle::serialize::ArchiveMode mode)
{
  assert(
        mode == elle::serialize::ArchiveMode::Input
    ||  mode == elle::serialize::ArchiveMode::Output
  );
  if (mode == elle::serialize::ArchiveMode::Input)
    return out << "Input";
  else
    return out << "Output";
}

