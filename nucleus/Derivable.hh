//
// ---------- header ----------------------------------------------------------
//
// project       nucleus
//
// license       infinit
//
// file          /home/mycure/infinit/nucleus/Derivable.hh
//
// created       julien quintard   [fri jul  8 10:42:21 2011]
// updated       julien quintard   [wed sep  7 14:54:59 2011]
//

#ifndef NUCLEUS_DERIVABLE_HH
#define NUCLEUS_DERIVABLE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

namespace nucleus
{

//
// ---------- classes ---------------------------------------------------------
//

  ///
  /// this class provides a nucleus-specific derivable class i.e a
  /// class for generating nucleus objects based on a product number.
  ///
  template <typename T>
  class Derivable:
    public elle::Derivable<T>
  {
  public:
    //
    // constructors & destructors
    //
    Derivable();
    Derivable(const elle::Product&,
	      const T&);
    Derivable(T&);
  };

}

//
// ---------- templates -------------------------------------------------------
//

#include <nucleus/Derivable.hxx>

#endif
