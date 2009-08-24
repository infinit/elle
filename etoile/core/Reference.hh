//
// ---------- header ----------------------------------------------------------
//
// project       infinit
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/core/Reference.hh
//
// created       julien quintard   [mon jul 27 10:19:21 2009]
// updated       julien quintard   [sun aug 23 10:42:58 2009]
//

#ifndef ETOILE_CORE_REFERENCE_HH
#define ETOILE_CORE_REFERENCE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/core/Contents.hh>

namespace etoile
{
  namespace core
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents the contents of a link.
    ///
    class Reference:
      public Contents
    {
    public:
      //
      // structures
      //

      //
      // methods
      //

      // XXX

      Status		Size(Natural64&) const;

      //
      // interfaces
      //

      // dumpable
      Status		Dump(const Natural32 = 0) const;

      // archivable
      Status		Serialize(Archive&) const;
      Status		Extract(Archive&);

      //
      // attributes
      //
      String		target;
    };

  }
}

#endif
