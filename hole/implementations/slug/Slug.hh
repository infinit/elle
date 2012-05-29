#ifndef HOLE_IMPLEMENTATIONS_SLUG_SLUG_HH
# define HOLE_IMPLEMENTATIONS_SLUG_SLUG_HH

# include <elle/types.hh>
# include <hole/implementations/slug/Machine.hh>

namespace hole
{
  namespace implementations
  {
    ///
    /// XXX
    ///
    namespace slug
    {

//
// ---------- classes ---------------------------------------------------------
//

      ///
      /// XXX
      ///
      ///
      class Slug
      {
      public:
        //
        // static attributes
        //
        static Machine*         Computer;
      };

    }
  }
}

//
// ---------- includes --------------------------------------------------------
//

# include <hole/implementations/slug/Bank.hh>
# include <hole/implementations/slug/Cluster.hh>
# include <hole/implementations/slug/Host.hh>
# include <hole/implementations/slug/Implementation.hh>
# include <hole/implementations/slug/Machine.hh>
# include <hole/implementations/slug/Manifest.hh>
# include <hole/implementations/slug/Neighbourhood.hh>

#endif
