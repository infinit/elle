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
// updated       julien quintard   [mon may  3 15:56:14 2010]
//

#ifndef ETOILE_WALL_INTERFACE_HH
#define ETOILE_WALL_INTERFACE_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/Elle.hh>

#include <etoile/wall/Object.hh>
#include <etoile/wall/Access.hh>
#include <etoile/wall/Directory.hh>

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
      static elle::Status	Initialize();
      static elle::Status	Clean();

      //
      // static attributes
      //
      static elle::Routine*	Callbacks[etoile::Tags];
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
      elle::Callback< _parameters_ >*	_callback_ =			\
        new elle::Callback< _parameters_ >(&_function_);		\
									\
      if (elle::Network::Register<_tag_>(*_callback_) ==		\
          elle::StatusError)						\
        escape("unable to register the callback");			\
									\
      Interface::Callbacks[_tag_] = _callback_;				\
    } while (false)

  }
}

#endif
