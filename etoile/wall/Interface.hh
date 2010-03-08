//
// ---------- header ----------------------------------------------------------
//
// project       etoile
//
// license       infinit
//
// file          /home/mycure/infinit/etoile/wall/Interface.hh
//
// created       julien quintard   [wed mar  3 18:24:27 2010]
// updated       julien quintard   [thu mar  4 21:13:30 2010]
//

#ifndef ETOILE_WALL_INTERFACE_HH
#define ETOILE_WALL_INTERFACE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/wall/Types.hh>
#include <etoile/wall/User.hh>
#include <etoile/wall/Object.hh>

#include <etoile/Manifest.hh>

namespace etoile
{
  namespace wall
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    class Interface
    {
    public:
      //
      // static methods
      //
      static Status	Initialize();
      static Status	Clean();

      //
      // static attributes
      //
      static Callback*		Callbacks[::etoile::Tags];
    };

//
// ---------- macro-functions -------------------------------------------------
//

///
/// XXX
///
#define InterfaceRegister(_tag_, _function_, _parameters_...)		\
  do									\
    {									\
      Interface::Callbacks[_tag_] =					\
        new Function<_parameters_>(&_function_);			\
									\
      if (Network::Register<_tag_>(*Interface::Callbacks[_tag_]) ==	\
          StatusError)							\
        escape("unable to register the callback");			\
    } while (false)

  }
}

#endif
