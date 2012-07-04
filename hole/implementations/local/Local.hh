#ifndef HOLE_IMPLEMENTATIONS_LOCAL_LOCAL_HH
# define HOLE_IMPLEMENTATIONS_LOCAL_LOCAL_HH

# include <hole/implementations/local/fwd.hh>

namespace hole
{
  namespace implementations
  {
    ///
    /// this namespace contains everything related to the local implementation.
    ///
    /// this implementation basically stores every block on the node's
    /// local storage device.
    ///
    namespace local
    {

      ///
      /// this class controls the local implementation.
      ///
      class Local
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

#endif
