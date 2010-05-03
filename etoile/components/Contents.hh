//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/components/Contents.hh
//
// created       julien quintard   [mon apr  5 15:14:23 2010]
// updated       julien quintard   [mon may  3 12:39:39 2010]
//

#ifndef ETOILE_COMPONENTS_CONTENTS_HH
#define ETOILE_COMPONENTS_CONTENTS_HH

namespace etoile
{
  namespace components
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class provides functionalities for manipulating contents.
    ///
    class Contents
    {
    public:
      //
      // static methods
      //
      template <typename T>
      static elle::Status	Open(T*);
      template <typename T>
      static elle::Status	Destroy(T*);
      template <typename T>
      static elle::Status	Close(T*);
    };

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <etoile/components/Contents.hxx>

#endif
