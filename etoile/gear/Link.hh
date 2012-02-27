//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// author        julien quintard   [fri aug 14 23:13:51 2009]
//

#ifndef ETOILE_GEAR_LINK_HH
#define ETOILE_GEAR_LINK_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>
#include <nucleus/Nucleus.hh>

#include <etoile/gear/Object.hh>
#include <etoile/gear/Nature.hh>

#include <etoile/automaton/Link.hh>

#include <etoile/wall/Link.hh>

namespace etoile
{
  namespace gear
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// this class represents a file-specific context.
    ///
    /// this context derives the Object context and therefore benefits from
    /// all the object-related attributes plus the contents i.e the catalog
    /// in the case of a file.
    ///
    class Link:
      public Object
    {
    public:
      //
      // constants
      //
      static const Nature                       N = NatureLink;

      //
      // types
      //
      typedef wall::Link                        W;
      typedef automaton::Link                   A;
      typedef elle::Callback<
                elle::Status,
                elle::Parameters<
                  Link&
                  >
                >                               S;

      typedef nucleus::Reference                C;

      //
      // constructors & destructors
      //
      Link();
      ~Link();

      //
      // interfaces
      //

      // dumpable
      elle::Status      Dump(const elle::Natural32 = 0) const;

      // archivable
      elle::Status      Serialize(elle::Archive&) const;
      elle::Status      Extract(elle::Archive&);

      //
      // attributes
      //
      nucleus::Contents<C>*     contents;
    };

  }
}

#endif
