#ifndef ELLE_IO_DUMPABLE_HH
# define ELLE_IO_DUMPABLE_HH

#include <elle/types.hh>

namespace elle
{

  namespace io
  {

    ///
    /// this interface must be implemented by dumpable classes.
    ///
    class Dumpable
    {
    public:
      static const String               Shift;

      virtual ~Dumpable() {}

      virtual Status    Dump(const Natural32 shift = 0) const = 0;
    };

  }
}

#endif
