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
// updated       julien quintard   [thu mar 25 18:28:49 2010]
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
      static Routine*		Callbacks[::etoile::Tags];
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
      Callback< _parameters_ >*	_callback_ =				\
        new Callback< _parameters_ >(&_function_);			\
									\
      if (Network::Register<_tag_>(*_callback_) ==			\
          StatusError)							\
        escape("unable to register the callback");			\
									\
      Interface::Callbacks[_tag_] = _callback_;				\
    } while (false)

  }
}

//
// ---------- includes --------------------------------------------------------
//

#include <etoile/wall/Wall.hh>

#endif
