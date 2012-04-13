//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [fri aug 26 13:05:26 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_SLUG_CLUSTER_HH
#define HOLE_IMPLEMENTATIONS_SLUG_CLUSTER_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <hole/implementations/slug/Host.hh>
#include <hole/implementations/slug/Neighbourhood.hh>

#include <elle/idiom/Close.hh>
# include <list>
#include <elle/idiom/Open.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Cluster:
        public elle::Object
      {
      public:
        //
        // types
        //
        typedef std::list<elle::Locus>                  Container;
        typedef typename Container::iterator            Iterator;
        typedef typename Container::const_iterator      Scoutor;

        //
        // constructors & destructors
        //
        ~Cluster();

        //
        // methods
        //
        elle::Status            Create(const Neighbourhood&);

        //
        // interfaces
        //

        // object
        declare(Cluster);

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        // archivable
        elle::Status            Serialize(elle::Archive&) const;
        elle::Status            Extract(elle::Archive&);

        //
        // attributes
        //
        Container               container;
      };

    }
  }
}

#endif
