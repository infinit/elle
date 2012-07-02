#ifndef ELLE_RADIX_ALLEGE_HH
#define ELLE_RADIX_ALLEGE_HH

#include <elle/types.hh>

// XXX[class to delete]

namespace elle
{
  namespace radix
  {

    ///
    /// default Allege structure.
    ///
    template<Boolean>
    struct Allege;

    ///
    /// specialized Allege class for 'true' so that the 'false'
    /// specialization remains undefined.
    ///
    /// this template structure can be used to generate assertions at
    /// compile time. thus, Allege<>(_condition_) is very much equivalent
    /// to assert(_condition_) but at compile time.
    ///
    template<>
    struct Allege<true>
    {
    };

  }
}

#endif
