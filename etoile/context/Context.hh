//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit (c)
//
// file          /home/mycure/infinit/etoile/context/Context.hh
//
// created       julien quintard   [fri aug 14 22:36:10 2009]
// updated       julien quintard   [sat jan 30 20:56:15 2010]
//

#ifndef ETOILE_CONTEXT_CONTEXT_HH
#define ETOILE_CONTEXT_CONTEXT_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

///
/// this file is directly inserted to avoid pre-processing issues.
///
#include <etoile/journal/Set.hh>

namespace etoile
{
  ///
  /// XXX
  ///
  namespace context
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Context
    {
    public:
      //
      // interface
      //

      ///
      /// this method is called by the journal in order for the specific
      /// context to register its blocks in the journal container.
      ///
      /// note that the register methods should always make sure that
      /// immutable blocks arrive first in the set so that the system
      /// publishes the data blocks before publishing the updated object.
      /// indeed, otherwise, the object would be updated, pointing to
      /// inexisting data blocks i.e not published yet.
      ///
      virtual Status	Register(journal::Set::Container&) = 0;
    };

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/context/Object.hh>
#include <etoile/context/Directory.hh>

#endif
