#ifndef HOLE_IMPLEMENTATIONS_SLUG_CLUSTER_HH
# define HOLE_IMPLEMENTATIONS_SLUG_CLUSTER_HH

# include <elle/types.hh>
# include <elle/radix/Object.hh>
# include <elle/network/Locus.hh>

# include <hole/implementations/slug/fwd.hh>

# include <elle/idiom/Close.hh>
#  include <list>
# include <elle/idiom/Open.hh>

namespace hole
{
  namespace implementations
  {
    namespace slug
    {

      ///
      /// XXX
      ///
      class Cluster:
        public elle::radix::Object
      {
      public:
        //
        // types
        //
        typedef std::list<elle::network::Locus>                  Container;
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

        //
        // attributes
        //
        Container               container;
      };

    }
  }
}

#include <hole/implementations/slug/Cluster.hxx>

#endif
