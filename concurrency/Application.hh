//
// ---------- header ----------------------------------------------------------
//
// project       elle
//
// license       infinit
//
// file          /home/mycure/infinit/elle/concurrency/Application.hh
//
// created       julien quintard   [mon mar 15 20:37:49 2010]
// updated       julien quintard   [tue mar 16 11:45:23 2010]
//

#ifndef ELLE_CONCURRENCY_APPLICATION_HH
#define ELLE_CONCURRENCY_APPLICATION_HH

//
// ---------- includes --------------------------------------------------------
//

#include <elle/core/Core.hh>
#include <elle/misc/Misc.hh>

#include <elle/concurrency/Accord.hh>
#include <elle/concurrency/Identifier.hh>

#include <elle/idiom/Close.hh>
# include <QCoreApplication>
# include <list>
#include <elle/idiom/Open.hh>

namespace elle
{
  namespace concurrency
  {

//
// ---------- classes ---------------------------------------------------------
//

    ///
    /// XXX
    ///
    /// XXX un peu pourri car si on wait E1 puis un traite un evenement
    /// qui lui veut attendre E2 et que par la suite on recoit E1, on ne
    /// peut pas resume le handler qui attendait E1 sans qu'on ait recu
    /// E2 :(
    ///
    /// meme avec des setjmp/longjmp ca ne pourrait pas marcher car on
    /// veut pouvoir sauter forward/backward dans la stack pour resume
    /// une frame. l'ideal serait d'allouer une stack specifique ce qui
    /// finalement ne serait pas trop critique puisque pour les threads
    /// ca le fait egalement. la question c'est surtout comment le faire
    /// proprement et de maniere portable car hors de question de mettre
    /// de l'asm dans le code.
    ///
    class Application
    {
    public:
      //
      // types
      //
      typedef std::pair<const Identifier, Void*>	Value;
      typedef std::list<Value*>				Container;
      typedef Container::iterator			Iterator;
      typedef Container::const_iterator			Scoutor;

      //
      // static methods
      //
      static Status		Initialize(); // XXX
      static Status		Clean(); // XXX

      static Status		Setup(int,
				      char**);

      static Status		Process();

      template <typename T>
      static Status		Wait(const Identifier&,
				     T*&);
      template <typename T>
      static Status		Awake(const Identifier&,
				      T*);

      //
      // static attributes
      //
      static Accord		Control;

      static Container		Events;

      //
      // constructors & destructors
      //
      Application(int,
		  char**);

      //
      // attributes
      //
      ::QCoreApplication	core;
    };

//
// ---------- extern ----------------------------------------------------------
//

    extern Application*				application;

  }
}

//
// ---------- templates -------------------------------------------------------
//

#include <elle/concurrency/Application.hxx>

#endif
