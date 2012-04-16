//
// ---------- header ----------------------------------------------------------
//
// project       hole
//
// license       infinit
//
// author        julien quintard   [thu aug 25 10:56:32 2011]
//

#ifndef HOLE_IMPLEMENTATIONS_SLUG_NEIGHBOURHOOD_HH
#define HOLE_IMPLEMENTATIONS_SLUG_NEIGHBOURHOOD_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/types.hh>

#include <hole/Label.hh>
#include <hole/implementations/slug/Host.hh>

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
      class Neighbourhood:
        public elle::Entity
      {
      public:
        //
        // types
        //
        typedef std::pair<const elle::network::Locus, Host*>     Value;
        typedef std::map<const elle::network::Locus, Host*>      Container;
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
                                    Host*);
        elle::Status            Exist(const elle::network::Locus&) const;
        elle::Status            Retrieve(const elle::network::Locus&,
                                         Host*&) const;
        elle::Status            Remove(const elle::network::Locus&);
        elle::Status            Locate(const elle::network::Locus&,
                                       Scoutor&) const;
        elle::Status            Locate(const elle::network::Locus&,
                                       Iterator&);

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
