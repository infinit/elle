//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [thu aug 25 10:56:32 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_CIRKLE_NEIGHBOURHOOD_HH
#define HOLE_IMPLEMENTATIONS_CIRKLE_NEIGHBOURHOOD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <hole/Label.hh>
#include <hole/implementations/cirkle/Neighbour.hh>

namespace hole
{
  namespace implementations
  {
    namespace cirkle
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      class Neighbourhood:
        public elle::radix::Entity
      {
      public:
        //
        // types
        //
        typedef std::map<const elle::network::Locus, Neighbour*> Container;
        typedef typename Container::iterator            Iterator;
        typedef typename Container::const_iterator      Scoutor;

        //
        // constructors & destructors
        //
        ~Neighbourhood();

        //
        // methods
        //
        elle::Status            Add(const elle::network::Locus&,
                                    Neighbour*);
        elle::Status            Exist(const elle::network::Locus&);
        elle::Status            Retrieve(const elle::network::Locus&,
                                         Neighbour*&);
        elle::Status            Remove(const elle::network::Locus&);
        elle::Status            Locate(const elle::network::Locus&,
                                       Iterator* = NULL);

        //
        // interfaces
        //

        // dumpable
        elle::Status            Dump(const elle::Natural32 = 0) const;

        //
        // attributes
        //
        Container               container;
      };

    }
  }
}

#endif
